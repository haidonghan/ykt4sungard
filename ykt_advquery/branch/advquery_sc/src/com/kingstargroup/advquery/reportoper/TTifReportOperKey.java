/*
 * WARNING: DO NOT EDIT THIS FILE. This is a generated file that is synchronized
 * by MyEclipse Hibernate tool integration.
 *
 * Created Wed Oct 19 14:02:33 CST 2005 by MyEclipse Hibernate Tool.
 */
package com.kingstargroup.advquery.reportoper;

import java.io.Serializable;

/**
 * A class representing a composite primary key id for the T_TIF_REPORT_OPER
 * table.  This object should only be instantiated for use with instances 
 * of the TTifReportOper class.
 * WARNING: DO NOT EDIT THIS FILE. This is a generated file that is synchronized 
 * by MyEclipse Hibernate tool integration.
 */
public class TTifReportOperKey
    implements Serializable
{
    /** The cached hash code value for this instance.  Settting to 0 triggers re-calculation. */
    private volatile int hashValue = 0;

    /** The value of the BALANCE_DATE component of this composite id. */
    private java.lang.String balanceDate;

    /** The value of the OPERATOR_CODE component of this composite id. */
    private java.lang.String operatorCode;

    /** The value of the SERI_TYPE component of this composite id. */
    private java.lang.Integer seriType;

    /** The value of the MAINDEVICE_ID component of this composite id. */
    private java.lang.Integer maindeviceId;

    /** The value of the DEVICE_ID component of this composite id. */
    private java.lang.Integer deviceId;

    /**
     * Simple constructor of TTifReportOperKey instances.
     */
    public TTifReportOperKey()
    {
    }

    /**
     * Returns the value of the balanceDate property.
     * @return java.lang.String
     */
    public java.lang.String getBalanceDate()
    {
        return balanceDate;
    }

    /**
     * Sets the value of the balanceDate property.
     * @param balanceDate
     */
    public void setBalanceDate(java.lang.String balanceDate)
    {
        hashValue = 0;
        this.balanceDate = balanceDate;
    }

    /**
     * Returns the value of the operatorCode property.
     * @return java.lang.String
     */
    public java.lang.String getOperatorCode()
    {
        return operatorCode;
    }

    /**
     * Sets the value of the operatorCode property.
     * @param operatorCode
     */
    public void setOperatorCode(java.lang.String operatorCode)
    {
        hashValue = 0;
        this.operatorCode = operatorCode;
    }

    /**
     * Returns the value of the seriType property.
     * @return java.lang.Integer
     */
    public java.lang.Integer getSeriType()
    {
        return seriType;
    }

    /**
     * Sets the value of the seriType property.
     * @param seriType
     */
    public void setSeriType(java.lang.Integer seriType)
    {
        hashValue = 0;
        this.seriType = seriType;
    }

    /**
     * Returns the value of the maindeviceId property.
     * @return java.lang.Integer
     */
    public java.lang.Integer getMaindeviceId()
    {
        return maindeviceId;
    }

    /**
     * Sets the value of the maindeviceId property.
     * @param maindeviceId
     */
    public void setMaindeviceId(java.lang.Integer maindeviceId)
    {
        hashValue = 0;
        this.maindeviceId = maindeviceId;
    }

    /**
     * Returns the value of the deviceId property.
     * @return java.lang.Integer
     */
    public java.lang.Integer getDeviceId()
    {
        return deviceId;
    }

    /**
     * Sets the value of the deviceId property.
     * @param deviceId
     */
    public void setDeviceId(java.lang.Integer deviceId)
    {
        hashValue = 0;
        this.deviceId = deviceId;
    }

    /**
     * Implementation of the equals comparison on the basis of equality of the id components.
     * @param rhs
     * @return boolean
     */
    public boolean equals(Object rhs)
    {
        if (rhs == null)
            return false;
        if (! (rhs instanceof TTifReportOperKey))
            return false;
        TTifReportOperKey that = (TTifReportOperKey) rhs;
        if (this.getBalanceDate() == null || that.getBalanceDate() == null)
        {
            return false;
        }
        if (! this.getBalanceDate().equals(that.getBalanceDate()))
        {
            return false;
        }
        if (this.getOperatorCode() == null || that.getOperatorCode() == null)
        {
            return false;
        }
        if (! this.getOperatorCode().equals(that.getOperatorCode()))
        {
            return false;
        }
        if (this.getSeriType() == null || that.getSeriType() == null)
        {
            return false;
        }
        if (! this.getSeriType().equals(that.getSeriType()))
        {
            return false;
        }
        if (this.getMaindeviceId() == null || that.getMaindeviceId() == null)
        {
            return false;
        }
        if (! this.getMaindeviceId().equals(that.getMaindeviceId()))
        {
            return false;
        }
        if (this.getDeviceId() == null || that.getDeviceId() == null)
        {
            return false;
        }
        if (! this.getDeviceId().equals(that.getDeviceId()))
        {
            return false;
        }
        return true;
    }

    /**
     * Implementation of the hashCode method conforming to the Bloch pattern with
     * the exception of array properties (these are very unlikely primary key types).
     * @return int
     */
    public int hashCode()
    {
        if (this.hashValue == 0)
        {
            int result = 17;
            int balanceDateValue = this.getBalanceDate() == null ? 0 : this.getBalanceDate().hashCode();
            result = result * 37 + balanceDateValue;
            int operatorCodeValue = this.getOperatorCode() == null ? 0 : this.getOperatorCode().hashCode();
            result = result * 37 + operatorCodeValue;
            int seriTypeValue = this.getSeriType() == null ? 0 : this.getSeriType().hashCode();
            result = result * 37 + seriTypeValue;
            int maindeviceIdValue = this.getMaindeviceId() == null ? 0 : this.getMaindeviceId().hashCode();
            result = result * 37 + maindeviceIdValue;
            int deviceIdValue = this.getDeviceId() == null ? 0 : this.getDeviceId().hashCode();
            result = result * 37 + deviceIdValue;
            this.hashValue = result;
        }
        return this.hashValue;
    }
}