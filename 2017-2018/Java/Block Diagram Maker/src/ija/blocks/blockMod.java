package ija.blocks;

import ija.connection.connection;

public class blockMod extends AbstractBlock{
    private connection input2;

    public blockMod(String type, connection c1,connection c2, int connID)
    {
    	super.input1=c1;
        this.input2=c2;
        this.output = new connection(type, 0.0, connID);
    }
   
    public void execute() {
        output.setValue(input1.getValue() % input2.getValue());
    }
}