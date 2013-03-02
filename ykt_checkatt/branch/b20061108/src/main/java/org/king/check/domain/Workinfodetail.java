package org.king.check.domain;
// Generated by MyEclipse - Hibernate Tools

import java.util.Set;
import org.apache.commons.lang.builder.CompareToBuilder;
import org.apache.commons.lang.builder.EqualsBuilder;
import org.apache.commons.lang.builder.HashCodeBuilder;
import org.apache.commons.lang.builder.ToStringBuilder;


/**
 * Workinfodetail generated by MyEclipse - Hibernate Tools
 */
public class Workinfodetail extends AbstractWorkinfodetail implements Comparable, java.io.Serializable {

    // Constructors

    /** default constructor */
    public Workinfodetail() {
    }

    
    /** full constructor */
    public Workinfodetail(String checkdate, String dutycheckdate,String infotype,Integer exceptionmins) {
        super(checkdate, dutycheckdate, infotype, exceptionmins);        
    }


	/**
	 * @see java.lang.Comparable#compareTo(Object)
	 */
	public int compareTo(Object object) {
		Workinfodetail myClass = (Workinfodetail) object;
		return new CompareToBuilder().toComparison();
	}


	/**
	 * @see java.lang.Object#equals(Object)
	 */
	public boolean equals(Object object) {
		if (!(object instanceof Workinfodetail)) {
			return false;
		}
		Workinfodetail rhs = (Workinfodetail) object;
		return new EqualsBuilder().appendSuper(super.equals(object)).isEquals();
	}


	/**
	 * @see java.lang.Object#hashCode()
	 */
	public int hashCode() {
		return new HashCodeBuilder(631201879, -1688611777).appendSuper(
				super.hashCode()).toHashCode();
	}


	/**
	 * @see java.lang.Object#toString()
	 */
	public String toString() {
		return new ToStringBuilder(this).append("dutycheckdate",
				this.getDutycheckdate()).append("checkdate",
				this.getCheckdate()).append("detailid", this.getDetailid())
				.append("infotype", this.getInfotype()).toString();
	}
   
}