/*
 * CS 61C Fall 2013 Project 1
 *
 * DoublePair.java is a class which stores two doubles and 
 * implements the Writable interface. It can be used as a 
 * custom value for Hadoop. To use this as a key, you can
 * choose to implement the WritableComparable interface,
 * although that is not necessary for credit.
 */

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.util.*;

import org.apache.hadoop.io.Writable;
import org.apache.hadoop.io.WritableComparable;

public class DoublePair implements WritableComparable{
    // Declare any variables here
    private double occur;
    private double dValue; 

    /**
     * Constructs a DoublePair with both doubles set to zero.
     */
    public DoublePair() {
        // YOUR CODE HERE
        occur = 0;
        dValue = 0;

    }

    /**
     * Constructs a DoublePair containing double1 and double2.
     */ 
    public DoublePair(double double1, double double2) {
        // YOUR CODE HERE
        occur = double1;
        dValue = double2;

    }

    /**
     * Returns the value of the first double.
     */
    public double getDouble1() {
        // YOUR CODE HERE
        return occur;
        
    }

    /**
     * Returns the value of the second double.
     */
    public double getDouble2() {
        // YOUR CODE HERE
        return dValue;
        
    }

    /**
     * Sets the first double to val.
     */
    public void setDouble1(double val) {
        // YOUR CODE HERE
        this.occur = val;

    }

    /**
     * Sets the second double to val.
     */
    public void setDouble2(double val) {
        // YOUR CODE HERE
        this.dValue = val;

    }

    /**
     * write() is required for implementing Writable.
     */
    public void write(DataOutput out) throws IOException {
        // YOUR CODE HERE
        out.writeDouble(this.getDouble1());
        out.writeDouble(this.getDouble2());

    }

    /**
     * readFields() is required for implementing Writable.
     */
    public void readFields(DataInput in) throws IOException {
        // YOUR CODE HERE
        this.setDouble1(in.readDouble());
        this.setDouble2(in.readDouble());
    }

    public double coRate () {
        if (this.getDouble2() > 0) {
            return (this.getDouble2()*Math.pow(Math.log(this.getDouble2()), 3))/this.getDouble1();
        } else {
            return 0;
        }
    }

    public int hashCode() {
        Double hash = new Double(this.coRate());
        return hash.hashCode();
    }

    public int compareTo(Object O) {
        if (this.coRate() < ((DoublePair) O).coRate()) {
            return 1;
        } else if (this.coRate() > ((DoublePair) O).coRate()) {
            return -1;
        } else {
            return 0;
        }
    }

    public static void main (String[] args) {

    }
}
