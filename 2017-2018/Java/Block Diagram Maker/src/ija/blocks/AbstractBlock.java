package ija.blocks;

import ija.connection.connection;
import javafx.scene.shape.Rectangle;
import javafx.scene.paint.ImagePattern;
import javafx.scene.shape.Shape;
import javafx.scene.layout.GridPane;

public abstract class AbstractBlock extends GridPane {
    protected int BlockID;
    protected connection input1 = null;
    protected connection output = null;
    protected String nameBlock = "";

    public connection getOutput(){
        return this.output;
    }
    public connection getInput1() {
        return input1;
    }
    /* After creating schema and checking which block is connected to actual, this block will set as input1 */
    public void setInput1(connection in1){
        this.input1 = in1;
    }

    public void setNameBlock(String name){
        this.nameBlock = name;
    }
    public String getNameBlock(){
        return this.nameBlock;
    }

    public abstract void execute();

}