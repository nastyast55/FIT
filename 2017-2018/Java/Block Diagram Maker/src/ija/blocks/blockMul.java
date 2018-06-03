package ija.blocks;
import ija.connection.connection;

public class blockMul extends AbstractBlock{
    private connection input2;

    /* While user creates blocks */
    public blockMul(String type, int connID){
        this.output = new connection(type, connID);
    }

    public blockMul (String type, connection c1,connection c2, int connID)
    {
    	super.input1=c1;
        this.input2=c2;
        this.output = new connection(type, 0.0, connID);
    }
    public connection getInput2() {
        return input2;
    }
    public void setInput2(connection in2){
        this.input2 = in2;
    }

    public void execute() {
        output.setValue(input1.getValue()*input2.getValue());
    }
}