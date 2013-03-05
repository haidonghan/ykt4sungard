
#include "taskdef.h"
#include "taskqueue.h"
#include "task_scheduler.h"
#include "ksgexception.h"
#include "deviceinterface.h"
#include "device.h"
#include "ksgateway.h"
#include "taskcode.h"
#include "bcc_server.h"
#include <ace/ACE.h>
#include <ace/Log_Msg.h>

//////////////////////////////////////////////////////////////////////////
void KSGTask_Pool_CheckPolicy::OnNewCheck(TaskType *p)
{
	//
}
void KSGTask_Pool_CheckPolicy::OnDeleteCheck(TaskType *p)
{
	if(!p)
		return;
	KsgGetGateway()->GetTaskManager()->free_task_id(p->GetTaskId());
}
/////////////////////////////////////////////////////////////////////////
// KSGTaskManager
ACE_Thread_Mutex KSGTaskManager::_s_mutex;
//! constructor
KSGTaskManager::KSGTaskManager():_generator(90000)
{
	//_check = new TaskAllocCheck(this);
	//_pool.Attach(_check);
}
//! destructor
KSGTaskManager::~KSGTaskManager()
{
	Clear();
	/*
	if(_check)
	{
		delete _check;
	}
	*/
}
void KSGTaskManager::Clear()
{
	_tasks.clear();
}

KSGTaskManager::PtrType KSGTaskManager::NewTask(int taskId,int code,const std::string& content
					,KSGDeviceNode* node,TaskPriority pri /*= ptNormal*/,void *arg)
{
	// ������һ���µ�����ʱ,��Ҫ���� TaskPool �� singal ����ȥ���ѿ����Ѿ��ڵȴ����߳�
	// �ȴ�������߳�����һ�α�����ִ��ʱ,�������õ�����
	PtrType null_task((TaskType*)NULL);
	PtrType ret((TaskType*)NULL);
	int res;
	KSG_Task_Queue *queue = NULL;
	{
		ACE_GUARD_RETURN(ACE_Thread_Mutex,mon,_s_mutex,null_task);
		TaskType * task = FindTask(taskId);
		if(task != NULL)
		{
			// ��������Ѿ�����, �׳��쳣
			throw TaskAlreadyExistsException();
		}
		else
		{
			queue = node->owner_queue();
			if(!queue)
				throw NullPointerException();
			try
			{
				ret = _pool.Alloc(taskId,code,content);
				_tasks.insert(TaskMapType::value_type(ret->GetTaskId()
													,ret.get()));
				//ACE_DEBUG((LM_INFO,"����ָ�����id[%d]",ret->GetTaskId()));
				
				ret->SetDeviceNode(node);
				ret->private_data(arg);
				// 
				res = 0;
				switch(pri)
				{
				case ptReal:
					res = queue->insert_task(ret);
					break;
				case ptPreempt:
					// TODO : support preemptive task
					ACE_DEBUG((LM_DEBUG,"��ռʽ����taskid[%d]",taskId));
					res = queue->push_preemptive_task(ret);
					break;
				default:
				case ptNormal:
					res = queue->push_task(ret);
					ACE_DEBUG((LM_DEBUG,"����������[%d]",queue->count_of_tasks()));
					break;
				}
				if(res)
				{
					ACE_DEBUG((LM_ERROR,"����������ʧ��!!taskid[%d]",taskId));
					return null_task;
				}
			}
			catch(...)
			{
				return null_task;
			}
		}
	}
	std::string key = node->owner_queue()->key_name();
	Task_Queue_Pool::instance()->singal_queue(key,queue);
	return ret;
}

void KSGTaskManager::free_task_id(int id)
{
	TaskMapType::iterator i = _tasks.find(id);
	if(i != _tasks.end())
	{
		//ACE_DEBUG((LM_DEBUG,"�ͷ�ָ�����id[%d]",id));
		void * p = (i->second)->private_data();
		if(p)
		{
			KSG_BCC_Svr_Handler::free_handler((KSG_BCC_Svr_Handler*)p);
		}
		_tasks.erase(i);
	}
}
KSGTaskManager::TaskType* KSGTaskManager::FindTask(int taskId)
{
	TaskMapType::const_iterator i = _tasks.find(taskId);
	return (i == _tasks.end()) ? NULL : (i->second);
}

long KSGTaskManager::GenTaskId()
{
	int ret = _generator.GetNextVal();
	return -ret;
}

KSGTaskManager::PtrType KSGTaskManager::AddSysTask(int code,const std::string& content
								   ,KSGDeviceNode* node,TaskPriority pri /*= ptNormal*/,void *arg)
{
	int taskid;
	switch(code)
	{
		case TK_COLLSERIAL_TASK:
			taskid = node->GetDevId()*10 + 1;
			taskid = -taskid;
			break;
		case TK_HEARTBEAT_TASK:
			taskid = node->GetDevId()*10 + 2;
			taskid = -taskid;
			break;
		case TK_BATCH_DL_BLKCARD:
			taskid = node->GetDevId()*10 + 3;
			taskid = -taskid;
			break;
		case 930118:
			taskid = node->GetDevId()*10 + 4;
			taskid = -taskid;
			break;
		default:
			taskid = GenTaskId();
			break;
	}
	PtrType task = NewTask(taskid,code,content,node,pri,arg);
	return task;
}

void KSGTaskManager::FreeTask(TaskType* task)
{
	ACE_GUARD(ACE_Thread_Mutex,mon,_s_mutex);
	assert(task != NULL);
	TaskMapType::iterator i = _tasks.find(task->GetTaskId());
	if(i != _tasks.end())
	{
		//ACE_DEBUG((LM_DEBUG,"�ͷ�ָ�����id[%d]",task->GetTaskId()));
		_tasks.erase(i);
	}
}

int KSGTaskManager::GetTaskCount()
{
	ACE_GUARD_RETURN(ACE_Thread_Mutex,mon,_s_mutex,-1);
	return _tasks.size();
}
//////////////////////////////////////////////////////////////////////////
// KSGTaskQueueManager
void KSGTaskQueueManager::AddTask(TaskType& task)
{
	KSGDeviceNode* node = task->GetDeviceNode();
	if(node == NULL)
		throw KSGException();
	TaskGroupType* group= FindDeviceTaskGroup(node);

	if(!group)
	{
		throw KSGException();
	}
	if(group->count() > KsgGetGateway()->GetConfig()->_taskQueueMaxWaiting 
		&& KsgGetGateway()->GetConfig()->_taskQueueMaxWaiting != -1)
	{
		throw TaskQueueExceedException();
	}
	switch(task->GetPriority())
	{
	case ptNormal:
		group->append(task);
		break;
	case ptReal:
		group->insert(task);
		break;
	case ptPreempt:
		// ������ռʽ���� 
		break;
	}
}

KSGTaskQueueManager::TaskGroupType* KSGTaskQueueManager::NewTaskGroup(int id)
{
	TaskGroupType* grp = new TaskGroupType(id);
	_groups.insert(GroupMapType::value_type(id,grp));
	return grp;
}

void KSGTaskQueueManager::FreeTaskGroup(GroupMapType::value_type& value)
{
	TaskGroupType* grp = value.second;
	if(grp)
	{
		delete grp;
	}
}
KSGTaskQueueManager::TaskGroupType* KSGTaskQueueManager::FindDeviceTaskGroup(KSGDeviceNode* node)
{
	DeviceGrpMapType::const_iterator i = _devgrps.find(node->GetDevId());
	if(i != _devgrps.end())
	{
		GroupMapType::const_iterator j = _groups.find(i->second);
		if(j != _groups.end())
		{
			return j->second;
		}
	}
	return NULL;
}
void KSGTaskQueueManager::RecursiveAddDevice(KSGDeviceNode* parent,int grpId)
{
	if(!parent)
	{
		return;
	}
	_devgrps.insert(DeviceGrpMapType::value_type(parent->GetDevId(),grpId));
	KSGDeviceNode* c = parent->GetFirstChild();
	while(c)
	{
		_devgrps.insert(DeviceGrpMapType::value_type(c->GetDevId(),grpId));
		RecursiveAddDevice(c,grpId);
		c = c->GetNextSibling();
	}
}

//////////////////////////////////////////////////////////////////////////
// TaskExecScheduler

void TaskExecScheduler::Run()
{
	bool notask = false;
	std::string retmsg;
	{
		ACE_DEBUG((LM_DEBUG,"ָ�����ִ��ָ��..."));
		GlobalTaskManager::PtrType task = _group->peek();
		if(task)
		{
			// ִ��ָ��
			KSGDeviceNode* node = task->GetDeviceNode();
			if(node == NULL)
			{
				ACE_DEBUG((LM_DEBUG,"ָ��[%d]û��ִ���豸",task->GetTaskId()));
				return;
			}
			int ret = TASK_ERR_COMMON;
			try
			{
				task->GetParams();
				task->SetNeedResponse();
				ret = node->ExecuteTask(task.operator->());
				ACE_DEBUG((LM_DEBUG,"ִ��ָ��[%d]dev[%d]ret[%d]",
					task->GetTaskId(),node->GetDevId(),ret));
			}
			catch(...)
			{
				// ָ���ʽ����ȷ
			}
			if(TASK_SUCCESS == ret)
			{
				// ִ��ָ��ɹ�
				retmsg = "�ɹ�";
			}
			else if( TASK_ERR_NOT_SUPPORT == ret )
			{
				retmsg = "�豸��֧��ָ��";
				ret = TASK_SUCCESS;
			}
			else if( TASK_ERR_TIMEOUT == ret)
			{
				// ִ�г�ʱ
				retmsg = "ִ�г�ʱ";
			}
			else if(TASK_ERR_EXECUTE == ret)
			{
				retmsg = "ִ��ʧ��";
			}
			else if(TASK_ERR_CONNECT == ret)
			{
				// �����豸ʧ��
				ACE_DEBUG((LM_DEBUG,"�޷����ӵ��豸�����̵߳ȴ�5�������"));
				retmsg = "�����豸ʧ��";
				KSGThreadUtil::Sleep(5000);
			}
			else
			{
				// ָ��ִ��ʧ��
				// ������ָ��
			}
			if(task->NeedResponse())
			{
				// ���µ���̨
				DRTPPoolType::SmartObject obj = KsgGetDrtpPool()->Alloc();
				obj->SetRequestHeader(KsgGetGateway()->GetConfig()->_taskResponseFuncNo);
				obj->AddField(F_LVOL1,task->GetTaskId());
				obj->AddField(F_LVOL4,ret);
				obj->AddField(F_VSMESS,retmsg.c_str());
				
				KSGTaskExecutor* exec = NULL;
				try
				{
					exec = KSGTaskExecutorFactory::instance()->Create(
						task->GetTaskCode());
					if(exec)
					{
						if(exec->ExecuteTask(obj.operator ->(),task.operator->()))
						{
							ACE_DEBUG((LM_ERROR,"ָ��ִ����ִ��ʧ��!"));
						}
					}
				}
				catch(...)
				{
					//
				}
				if(obj->Connect() || obj->SendRequest(300))
					ACE_DEBUG((LM_ERROR,"ָ��Ӧ��ʧ��...taskid[%d]",task->GetTaskId()));
			}
		}
		else
			notask = true;
		ACE_DEBUG((LM_DEBUG,"ָ�����ִ��ָ�����"));
	}
	if(notask)
	{
		// û��ָ��ִ��Ҫ���� Idle ״̬
		_group->wait();
	}
}

////////////////////////////////////////////////////////////////////////
//
int KSGDeviceGroupScheduler::AddNewDeviceGroup(KSGTaskQueueManager::TaskGroupType* grp)
{
	if( grp != NULL )
	{
		TaskExecScheduler * schd = _pool.construct(grp);
		this->NewScheduler(schd);
		_taskexecs.push_back(schd);
		return 0;
	}
	return -1;
}

//void KSGDeviceGroupScheduler::ClearAllExec()
//{
//	std::for_each(_taskexecs.begin(),_taskexecs.end(),
//		boost::bind(&KSGDeviceGroupScheduler::FreeTaskExecs,this,_1));
//}
void KSGDeviceGroupScheduler::FreeScheduler(KSGScheduler* exec)
{
	TaskExecScheduler * p = dynamic_cast<TaskExecScheduler*>(exec);
	if(p)
		_pool.free(p);
}

//////////////////////////////////////////////////////////////////////////
// KSGTaskQueueScheduler
KSGTaskQueueScheduler::KSGTaskQueueScheduler(int thr_max_cnt)
	:_thread_max_count(thr_max_cnt),_is_terminated(true),_group_current_id(0)
{
}
KSGTaskQueueScheduler::~KSGTaskQueueScheduler()
{
}

int KSGTaskQueueScheduler::terminate()
{
	return -1;
}

int KSGTaskQueueScheduler::peek_task_queue(task_queue_type *queue)
{
	return -1;
}

int KSGTaskQueueScheduler::push_task_queue(task_queue_type *queue, KSGTaskQueueStatus status)
{
	return -1;
}

int KSGTaskQueueScheduler::add_task(GlobalTaskManager::PtrType task)
{
	return -1;
}

int KSGTaskQueueScheduler::setup_all_queue()
{
	return -1;
}

int KSGTaskQueueScheduler::svc()
{
	return -1;
}

int KSGTaskQueueScheduler::add_device_group(KSGDeviceNode *node)
{
	if(contain_device_in_queue(node))
	{
		return node->GetDevId();
	}
	task_queue_ptr_type queue = new_task_queue();
	if(queue == NULL)
		return -1;
	// TODO : ��� queue �ĳ�ʼ������
	//
	
	_all_queue->push_back(queue);

	return 0;
}

int KSGTaskQueueScheduler::recursive_add_device(KSGDeviceNode *node,int group_id)
{
	if(contain_device_in_queue(node))
		return node->GetDevId();
	_dev_grp_queue.insert( device_task_queue_map_type::value_type(node->GetDevId(),group_id));
	KSGDeviceNode * next = node->GetFirstChild();
	int ret;
	while(next != NULL)
	{
		ret = recursive_add_device(next,group_id);
		if(ret)
			return ret;
		next = next->GetNextSibling();
	}
	return 0;
}

KSGTaskQueueScheduler::task_queue_ptr_type KSGTaskQueueScheduler::new_task_queue()
{
	return _queue_pool.construct(_group_current_id++);
}
inline bool KSGTaskQueueScheduler::contain_device_in_queue(KSGDeviceNode *node)
{
	return (_dev_grp_queue.find(node->GetDevId()) == _dev_grp_queue.end()) ? false : true;
}
