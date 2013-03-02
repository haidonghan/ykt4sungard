package com.kingstargroup.advquery.hibernate.dao;

import java.util.Iterator;
import java.util.List;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.hibernate.Hibernate;
import org.hibernate.HibernateException;
import org.hibernate.Query;
import org.hibernate.SQLQuery;
import org.hibernate.Session;

import com.kingstargroup.advquery.customer.TCifCustomer;
import com.kingstargroup.advquery.hibernate.HibernateSessionFactory;
public class AccountDAO extends BasicDAO {
	/**
	 * Description:singleton pattern
	 * @return
	 * Modify History:
	 */
	public static AccountDAO getInstance() {
		if (_instance == null) {
			_instance = new AccountDAO();
		}
		return _instance;
	}
	
	public List getHandTrade(String custNo,String beginDate,String endDate)throws HibernateException{
		//todo
		return null;
	}
	/**
	 * Copyright (C), 2000-2006, Kingstar Co., Ltd.<br>
	 * File name: .java<br>
	 * Description: TradeSerialDAOȡ��ĳ���˺�������ϸ<br>
	 * Return: List<br>
	 * Modify History: <br>
	 * ��������    ������     ����ʱ��       ��������<br>
	 * ======================================<br>
	 *  ����      ����ΰ     2006-5-10  <br>
	 * @author   ����ΰ
	 * @version 
	 * @since 1.0
	 */
	public List getOneAccountDetail(String custNo,String beginDate,String endDate)throws HibernateException{
		Session s = getSession();
		StringBuffer sqlStr = new StringBuffer();
		sqlStr.append("select log.OPERATE_DATE, log.OPERATE_TIME, log.DEVICE_ID, log.RECORD_OPER,log.COMMENTS,log.OP_FEE")
		.append(" from YKT_CUR.T_TIF_TRADELOG log,YKT_CUR.T_AIF_ACCOUNT acc ,YKT_CUR.T_CIF_CUSTOMER cust")
		.append(" where log.ACT_ID=acc.ACCOUNT_ID and acc.CUSTOMER_ID=cust.CUT_ID and")
		.append(" OPERATE_DATE>=:begindate and OPERATE_DATE <=:enddate and cust.CUT_ID=:custno ")
		.append(" union all")
		.append(" select log.OPERATE_DATE, log.OPERATE_TIME, log.DEVICE_ID, log.RECORD_OPER,log.COMMENTS,log.OP_FEE")
		.append(" from YKT_HIS.T_TIF_TRADELOG_HIS log,YKT_CUR.T_AIF_ACCOUNT acc ,YKT_CUR.T_CIF_CUSTOMER cust")
		.append(" where log.ACT_ID=acc.ACCOUNT_ID and acc.CUSTOMER_ID=cust.CUT_ID and ")
		.append(" log.BAK_DATE>=:begindate and log.BAK_DATE <=:enddate and cust.CUT_ID=:custno ");
		
		try {
			SQLQuery q = s.createSQLQuery(sqlStr.toString());
			q.addScalar("OPERATE_DATE",Hibernate.STRING);
			q.addScalar("OPERATE_TIME",Hibernate.STRING);
			q.addScalar("DEVICE_ID",Hibernate.INTEGER);
			q.addScalar("RECORD_OPER",Hibernate.STRING);
			q.addScalar("COMMENTS",Hibernate.STRING);
			q.addScalar("OP_FEE",Hibernate.FLOAT);
			q.setString("begindate",beginDate);
			q.setString("enddate",endDate);
			q.setString("custno",custNo);
			return q.list();
		} catch (HibernateException he) {
			_log.error(he);
			return null;
		}	
	}
	
	public String getDevNameByDevId(int devid)throws HibernateException{
		Session s = getSession();
		String devname="";
		try {
			if (0!=devid){
				
			    String sqlString = " select  device_name from ykt_cur.t_pif_device where device_id=:devid " ;
			    SQLQuery q = s.createSQLQuery(sqlString.toString());
				q.setInteger("devid",devid);
				q.addScalar("device_name",Hibernate.STRING);
				List list  = q.list();
				if(list !=null  && list.size()>0){
					devname = list.get(0).toString();
				}
				return devname;
			}else{
				return "";
			}
		} catch (HibernateException he) {
			_log.error(he);
			return null;
		}
	}
	public String getCustIdBystuempNo(String stuempNo){
		String custId ="";
		Session session = HibernateSessionFactory.currentSession();
		try{
			Query query = session.createQuery(" from TCifCustomer where stuempNo = :stuempNo");
			query.setString("stuempNo",stuempNo);
			List list = query.list();
			Iterator iter = list.iterator();
			while (iter.hasNext()){
				TCifCustomer row = (TCifCustomer)iter.next();
				custId  = row.getCutId().toString();
				break;
			}
			return custId;
		} catch (HibernateException e) {
			_log.error(e);
			return custId;
		}finally{
			HibernateSessionFactory.closeSession();
		}
	}
	
	private static AccountDAO _instance;
	private Log _log = LogFactory.getLog(AccountDAO.class.getClass());
}