/************************************************************************/
/*                                                                      */
/************************************************************************/
#include "proactor_def.h"
#include "ksgateway.h"
#include "factory.h"
#include <ace/Proactor.h>
#include <ace/Proactor_Impl.h>
#include <ace/WIN32_Proactor.h>
#include <ace/POSIX_Proactor.h>

///////////////////////////////////////////////////////////////////////////
// KSG_Proactor_Task 
KSG_Proactor_Task::KSG_Proactor_Task():_max_task(0),_sem(NULL)
{
	//
}
KSG_Proactor_Task::~KSG_Proactor_Task()
{
	if(_sem)
	{
		_sem->release();
		delete _sem;
		_sem = NULL;
	}
}

int KSG_Proactor_Task::start_task(int max_task)
{
	_max_task = max_task;
	if(_max_task <= 0)
		_max_task = 1;
	if(_sem)
	{
		_sem->release();
		_sem->remove();
		delete _sem;
	}
	ACE_NEW_RETURN(_sem,ACE_Thread_Semaphore(_max_task),-1);
	for(int i = _max_task;i>0;i--)
		_sem->acquire();
	ACE_DEBUG((LM_DEBUG,"׼�������߳�..."));
	activate(THR_NEW_LWP|THR_JOINABLE,_max_task);
	return 0;
}

int KSG_Proactor_Task::stop_task()
{
	// �ر� Proactor ����
	ACE_Proactor::instance()->proactor_end_event_loop();
	// �ȴ������̱߳��ر�
	this->wait();
	// �ͷ����� acceptor �Լ� ����
	listener_map_type::const_iterator iter;
	for(iter = _listeners.begin(); iter != _listeners.end();++iter)
	{
		KSG_Proactor_Listener * pl = iter->second;
		if(pl)
		{
			ACE_DEBUG((LM_DEBUG,"�ر� Listener!"));
			pl->stop_listen();
			delete pl;
		}
	}
	_listeners.clear();
	return 0;
}

int KSG_Proactor_Task::svc()
{
	int ret;
	_sem->release(1);
	while(!ACE_Proactor::instance()->proactor_event_loop_done())
	{
		ACE_Time_Value t(3,0);
		ret = ACE_Proactor::instance()->handle_events(t);
		if(ret < 0)
			ACE_DEBUG((LM_ERROR,"Poractor ִ��ʧ��!"));
		else if(ret == 0)
            ACE_DEBUG((LM_DEBUG,"Proactor û���¼�����"));
		else
			ACE_DEBUG((LM_DEBUG,"Proactor ������������"));
	}
	ACE_DEBUG((LM_DEBUG,"Proactor �߳̽�������..."));
	return 0;
}

int KSG_Proactor_Task::register_listener(int id,ListenerCreatorType creator)
{
	_listener_classes.Register(id,creator);
	return 0;
}

int KSG_Proactor_Task::start_listener(int id)
{
	KSG_Proactor_Listener *pl = NULL;
	listener_map_type::const_iterator iter = _listeners.find(id);
	if( iter != _listeners.end())
		pl = iter->second;
	else
		pl = _listener_classes.Create(id);
	if(!pl)
		return -1;
	_listeners.insert(listener_map_type::value_type(id,pl));
	return pl->start_listen();
}
//////////////////////////////////////////////////////////////////////////
//
void KSG_Proactor_Check_Handler::handle_time_out(const ACE_Time_Value &tv, const void *act )
{
	// ��� 10 ����û�л������
	acceptor_->check_timeout_handle(60 * 10 * 1000);
}

//////////////////////////////////////////////////////////////////////////
//
KSG_Proactor_Acceptor::KSG_Proactor_Acceptor(size_t max_cli_cnt)
:_max_clients_cnt(max_cli_cnt),_total_cli(0)
{
	_clients.clear();
	_used_clts.clear();
}

KSG_Proactor_Acceptor::~KSG_Proactor_Acceptor()
{
	close();
}

void KSG_Proactor_Acceptor::remove(handler_type* ih)
{
	ACE_GUARD(ACE_Recursive_Thread_Mutex,ace_mon,*ACE_Static_Object_Lock::instance());
	// ��ʹ���б���ɾ��
	std::set<handler_type*>::iterator i = _used_clts.find(ih);
	if(i != _used_clts.end())
	{
		_used_clts.erase(i);
	}
	if(_total_cli > _max_clients_cnt)
	{
		_total_cli--;
		ACE_DEBUG((LM_NOTICE,"�������������,�ͷž����Դ, ʣ��[%d]",_total_cli));
		delete ih;
	}
	else
		_clients.push_back(ih);
}
void KSG_Proactor_Acceptor::close()
{
	ACE_GUARD(ACE_Recursive_Thread_Mutex,ace_mon,*ACE_Static_Object_Lock::instance());
	handler_type *ih;
	std::list<handler_type*>::iterator iter;
	for(iter = _clients.begin(); iter != _clients.end(); ++iter)
	{
		ih = *iter;
		delete ih;
	}
	_clients.clear();
	/*
	std::set<handler_type*>::iterator i;
	// ǿ�ƹر�
	for (i = _used_clts.begin();i != _used_clts.end();++i)
	{
		ih = *i;
		delete ih;
	}
	_used_clts.clear();
	*/
}

KSG_Proactor_Handler* KSG_Proactor_Acceptor::make_handler()
{
	ACE_DEBUG((LM_DEBUG,"��ʼ���� ACE HANDLER ..."));
	ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex,ace_mon,*ACE_Static_Object_Lock::instance(),NULL);
	handler_type* ih;
	if(_clients.empty())
	{
		ih = new_handler();
		_total_cli++;
		ih->set_handler_id(static_cast<int>(_total_cli));
		ACE_DEBUG((LM_DEBUG,"������ [%d] ���������",_total_cli));
	}
	else
	{
		ih = _clients.front();
		_clients.pop_front();
	}
	_used_clts.insert(ih);
	ACE_DEBUG((LM_DEBUG,"���� ACE HANDLER �ɹ�!"));
	return ih;
}
/*
int KSG_Proactor_Acceptor::open(const ACE_INET_Addr &address, size_t bytes_to_read, int pass_addresses, 
								int backlog, int reuse_addr, ACE_Proactor *proactor, 
								int validate_new_connection, int reissue_accept, int number_of_initial_accepts)
{
	return ACE_Asynch_Acceptor<KSG_Proactor_Handler>::open(address,bytes_to_read,1,backlog,
		reuse_addr,proactor,validate_new_connection,reissue_accept,number_of_initial_accepts);
}

int KSG_Proactor_Acceptor::validate_connection(const ACE_Asynch_Accept::Result &result, 
											   const ACE_INET_Addr & remote, const ACE_INET_Addr & local)
{

	return 0;
}
*/
void KSG_Proactor_Acceptor::check_timeout_handle(long msec)
{
	std::list<handler_type*> temp_hd;
	//std::copy(_used_clts.begin(),_used_clts.end();temp_set.begin());
	// ��鳬ʱ������
	std::set<handler_type*>::iterator i;
	ACE_Time_Value now_time = ACE_OS::gettimeofday();
	for (i = _used_clts.begin(); i != _used_clts.end();++i)
	{
		handler_type *ih = *i;
		if((now_time - ih->_last_io_time).sec() > msec / 1000)
		{
			temp_hd.push_back(ih);
		}
	}
	// gracefull shutdown connection
	std::list<handler_type*>::iterator i2 = temp_hd.begin();
	for (;i2 != temp_hd.end();++i2)
	{
		handler_type *ih = *i2;
		ih->free_handler();
	}
}
//////////////////////////////////////////////////////////////////////////

KSG_Proactor_Handler::~KSG_Proactor_Handler()
{
	//ACE_DEBUG((LM_DEBUG,"�ͷ� ACE ���� [%d]... ",_handler_id));
}

KSG_Proactor_Handler::KSG_Proactor_Handler(
	KSG_Proactor_Acceptor* acceptor /* = NULL */)
	:_acceptor(acceptor),_mblk(NULL),_handler_id(0),_send_count(0)
	,_recv_count(0)
{
	int sec = KsgGetGateway()->GetConfig()->_execTimeout/1000+8;
	sec = sec < 4 ? 4 : sec;
	_max_timeout_sec = ACE_Time_Value(sec);
	this->handle(ACE_INVALID_HANDLE);
}

void KSG_Proactor_Handler::addresses(const ACE_INET_Addr &remote_address, const ACE_INET_Addr &local_address)
{
	_remote_addr = remote_address;
}
void KSG_Proactor_Handler::open(ACE_HANDLE new_handle
								  , ACE_Message_Block &message_block)
{
	ACE_DEBUG((LM_DEBUG,"������ HANDLER[%d]",get_handler_id()));
	this->handle(new_handle);
	update_io_time();
	_can_be_closing = 1;
	if(_reader.open(*this) == -1 
		|| _writer.open(*this) == -1)
	{
		ACE_DEBUG((LM_ERROR,"�� ACE ����ʧ��!!!"));
		free_handler();
		return;
	}
	on_open(new_handle,message_block);
	/*
	if(proactor()->schedule_timer(*this,NULL,_max_timeout_sec) == -1)
	{
		ACE_DEBUG((LM_ERROR,"���ó�ʱ����ʧ��"));
		free_handler();
		return;
	}
	*/
	ACE_DEBUG((LM_DEBUG,"�����ӳɹ�..."));
}

int KSG_Proactor_Handler::free_handler()
{
	if(_send_count >0 || _can_be_closing == 0)
		return -1;
	//if(_can_be_closing || _send_count > 0)
	//	return -1;
	ACE_DEBUG((LM_INFO,"�黹 ACE HANDLER [%d] ... ",get_handler_id()));
	/*
	if(_reader.implementation())
		_reader.cancel();
	if(_writer.implementation())
		_writer.cancel();
	
	if(proactor())
		proactor()->cancel_timer(*this);
	*/
	if(this->handle() != ACE_INVALID_HANDLE)
	{
		//ACE_Message_Block mb(1);
		//_reader.read(mb,0);
		ACE_OS::shutdown(this->handle(),ACE_SHUTDOWN_BOTH);
		ACE_OS::closesocket(this->handle());
	}
	this->handle(ACE_INVALID_HANDLE);
	if(_mblk) 
	{
		_mblk->release();
		//delete _mblk;
	}
	_mblk = NULL;
	on_free_handler();
	_acceptor->remove(this);
	_can_be_closing = 0;
	ACE_DEBUG((LM_DEBUG,"�黹 ACE HANDLER ���"));
	return 0;
}

void KSG_Proactor_Handler::handle_time_out(const ACE_Time_Value &tv
											 , const void *act /* = NULL */)
{
	ACE_DEBUG((LM_ERROR," HANDLER [%d]��ʱ...",get_handler_id()));
	free_handler();
}

inline int KSG_Proactor_Handler::get_handler_id()
{
	return _handler_id;
}

void KSG_Proactor_Handler::set_handler_id(int id)
{
	_handler_id = id;
}
