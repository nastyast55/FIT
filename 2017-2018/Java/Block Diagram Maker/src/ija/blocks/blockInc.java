package ija.blocks;

import ija.connection.connection;

public class blockInc extends AbstractBlock{

    /* While user creates blocks */
    public blockInc(String type, int connID){
        this.output = new connection(type, connID);
    }
    //With connID
    public blockInc(String type, connection c1, int connID)
    {
        this.input1=c1;
        this.output = new connection(type, 0.0, connID);
    }
 
    public void execute() {
        output.setValue(input1.getValue()+1.0);
    }
}