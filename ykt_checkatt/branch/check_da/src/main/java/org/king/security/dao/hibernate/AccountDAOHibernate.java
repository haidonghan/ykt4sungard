package org.king.security.dao.hibernate;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.hibernate.Hibernate;
import org.king.check.domain.Toperlimit;
import org.king.framework.dao.BaseDAO;
import org.king.framework.dao.MyQuery;
import org.king.framework.util.MyUtils;
import org.king.security.dao.AccountDAO;
import org.king.security.domain.Account;
import org.king.security.domain.Role;

import java.io.Serializable;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

/**
 * Data access object (DAO) for domain model class Account.
 *
 * @author MyEclipse - Hibernate Tools
 * @see org.king.security.dao.hibernate.Account
 */
public class AccountDAOHibernate implements AccountDAO {

    private static final Log log = LogFactory.getLog(AccountDAOHibernate.class);

    protected void initDao() {
        //do nothing
    }

    private BaseDAO baseDAO;

    public void setBaseDAO(BaseDAO baseDAO) {
        this.baseDAO = baseDAO;
    }

    public List find(MyQuery myQuery) {
        log.debug("finding Account instance by myQuery");
        return baseDAO.findEntity(myQuery);
    }

    public List find(String query) {
        log.debug("finding Account instance by query");
        return baseDAO.findEntity(query);
    }

    public Account get(Serializable id) {
        log.debug("getting Account instance by id");
        Account account = (Account) baseDAO.getEntity(Account.class, id);
        if (account != null && !Hibernate.isInitialized(account.getRoles())) {
            Hibernate.initialize(account.getRoles());
        }
        return account;
    }

    public List getAll() {
        String allHql = "from Account";
        return baseDAO.findEntity(allHql);
    }

    public void save(Account transientInstance) {
        log.debug("saving Account instance");
        baseDAO.saveEntity(transientInstance);
    }

    public void update(Account transientInstance) {
        log.debug("updating Account instance");
        baseDAO.updateEntity(transientInstance);
    }


    public void delete(Account persistentInstance) {
        log.debug("deleting Account instance");

        //������ɫ
        Role role;
        Set roles = persistentInstance.getRoles();
        if ((roles != null) && (roles.size() > 0)) {
            for (Iterator i = roles.iterator(); i.hasNext();) {
                role = (Role) i.next();
                role.getAccounts().remove(persistentInstance);
                baseDAO.updateEntity(role);
            }
        }

        baseDAO.removeEntity(persistentInstance);
    }


    public Account findAccountByName(String name) {
        if (MyUtils.isBlank(name)) {
            return null;
        }

        List accounts = find("from Account a where a.enabled='1' and a.person.stuempno='" + name + "'");

        if (MyUtils.isBlank(accounts)) {
            return null;
        }

        Account account = (Account) accounts.get(0);
        if (account != null && !Hibernate.isInitialized(account.getRoles())) {
            Hibernate.initialize(account.getRoles());
        }
        return account;
    }

    public void saveAccountLimit(Toperlimit transientInstance) {
        baseDAO.saveEntity(transientInstance);

    }

    public void updateAccountLimit(Toperlimit transientInstance) {
        baseDAO.updateEntity(transientInstance);

    }

    public List getAccountLimit(String deptId, String operId) {
        List accountLimit = find("from Toperlimit where id.deptId='" + deptId + "' and id.operId='" + operId + "'");
        return accountLimit;
    }


    public List getAccountLimit(String operId) {
        List accountLimit = find("from Toperlimit where id.operId='" + operId + "'");
        return accountLimit;
    }

    public void deleteAccountLimit(Toperlimit transientInstance) {
        baseDAO.removeEntity(transientInstance);

    }

    public List findAccountRole(String accId, String roleId) {
        List accountRole = find("from IcoAccountRole i    where i.id.accountId='" + accId + "' and i.id.roleId='" + roleId + "'");
        return accountRole;
    }

    /**
     public void saveAccountRole(AccRole accrole) {
     baseDAO.saveEntity(accrole);

     }

     public void updateAccountRole(AccRole accrole) {
     baseDAO.updateEntity(accrole);

     }

     public void deleteAccountRole(AccRole accrole) {
     baseDAO.removeEntity(accrole);

     }
     **/


}