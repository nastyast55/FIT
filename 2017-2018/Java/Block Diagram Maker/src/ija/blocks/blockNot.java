package ija.blocks;
import ija.connection.connection;

public class blockNot extends AbstractBlock{

    /* While user creates blocks */
    public blockNot(String type, int connID){
        this.output = new connection(type, connID);
    }

    public blockNot(String type, connection c1, int connID)
    {
        super.input1=c1;
        this.output = new connection(type, 0.0, connID);
    }
    public void execute() {
        output.setValue(input1.getValue()*(-1));
    }
}