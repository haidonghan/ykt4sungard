package org.king.check.domain;



/**
 * AbstractTMakereportrecord generated by MyEclipse - Hibernate Tools
 */

public abstract class AbstractTMakereportrecord extends org.king.framework.domain.BaseObject implements java.io.Serializable {


    // Fields    

     private TMakereportrecordId id;
     private String reportDate;
     private String status;


    // Constructors

    /** default constructor */
    public AbstractTMakereportrecord() {
    }

	/** minimal constructor */
    public AbstractTMakereportrecord(TMakereportrecordId id) {
        this.id = id;
    }
    
    /** full constructor */
    public AbstractTMakereportrecord(TMakereportrecordId id, String reportDate, String status) {
        this.id = id;
        this.reportDate = reportDate;
        this.status = status;
    }

   
    // Property accessors

    public TMakereportrecordId getId() {
        return this.id;
    }
    
    public void setId(TMakereportrecordId id) {
        this.id = id;
    }

    public String getReportDate() {
        return this.reportDate;
    }
    
    public void setReportDate(String reportDate) {
        this.reportDate = reportDate;
    }

    public String getStatus() {
        return this.status;
    }
    
    public void setStatus(String status) {
        this.status = status;
    }
   








}