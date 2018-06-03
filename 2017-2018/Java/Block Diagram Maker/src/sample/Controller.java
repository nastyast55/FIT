package sample;

import ija.blocks.*;
import ija.connection.*;
import ija.files.*;


import javafx.beans.property.DoubleProperty;
import javafx.beans.property.SimpleDoubleProperty;
import javafx.embed.swing.SwingFXUtils;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.geometry.Bounds;
import javafx.geometry.VPos;
import javafx.geometry.HPos;
import javafx.scene.Cursor;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.control.Button;
import javafx.scene.control.MenuItem;
import javafx.scene.control.TextField;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.GridPane;
import javafx.scene.paint.Color;
import javafx.scene.paint.ImagePattern;
import javafx.scene.shape.*;
import javafx.stage.FileChooser;
import javafx.stage.Modality;
import javafx.stage.Stage;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;


public class Controller {
    boolean print = false, deleteFlag = false, complexFlag = false, connectFlag = false, loadFlag = false;
    String blockPrint = "";
    int countConn = 0, maxRow = 7, maxCol = 9;
    double inputDouble = 0.0;
    String inputComplex = "", inputStr = "";
    public int col1, row1, col2, row2;
    public int colRun = 0, rowRun = 0;
    AbstractBlock[] matrix;
    AbstractBlock[] inputMatrix = new AbstractBlock[maxRow*maxCol];
    ImageView[] imageMatrix = new ImageView[maxRow*maxCol];
    StringBuilder result = new StringBuilder();
    String newLine = System.getProperty("line.separator");

    @FXML
    private Button blockVal;
    @FXML
    private Button blockNot;
    @FXML
    private Button blockInc;
    @FXML
    private Button blockDec;
    @FXML
    private Button blockAdd;
    @FXML
    private Button blockSub;
    @FXML
    private Button blockMul;
    @FXML
    private Button blockDiv;
    @FXML
    private Button blockPow;
    @FXML
    private Button connectBlocks;
    @FXML
    private Button delete;
    // @FXML
    // private Button Clear;
    @FXML
    private MenuItem Save;
    @FXML
    private MenuItem Load;
    @FXML
    private Button Run;

    @FXML
    private Button Clean;
    @FXML
    private MenuItem Help;
    @FXML
    public TextArea outputText;
    @FXML
    private TextField setValue;
    @FXML
    private GridPane printGrid;


    public void setValueFromTextfield(){

        inputStr = setValue.getText();
        try{
            inputDouble = Double.parseDouble(inputStr);
        }catch (NumberFormatException ex){
            inputComplex = inputStr;
            complexFlag = true;
        }

        if(complexFlag){
            System.out.println("----------------------Input is complex-----------------");
        }

    }


    @FXML
    public void onButtonClicked(ActionEvent e) throws Exception {
        if (e.getSource().equals(blockVal)) {
            System.out.println("blockVal clicked.");
            print = true;
            blockPrint = "blockValue";
        } else if (e.getSource().equals(blockNot)) {
            print = true;
            blockPrint = "blockNot";
            System.out.println("blockNot clicked.");
        } else if (e.getSource().equals(blockInc)) {
            print = true;
            blockPrint = "blockInc";
            System.out.println("blockInc clicked.");
        } else if (e.getSource().equals(blockDec)) {
            print = true;
            blockPrint = "blockDec";
            System.out.println("blockDec clicked.");
        } else if (e.getSource().equals(blockAdd)) {
            print = true;
            blockPrint = "blockAdd";
            System.out.println("blockAdd clicked.");
        } else if (e.getSource().equals(blockSub)) {
            print = true;
            blockPrint = "blockSub";
            System.out.println("blockSub clicked.");
        } else if (e.getSource().equals(blockMul)) {
            print = true;
            blockPrint = "blockMul";
            System.out.println("blockMul clicked.");
        } else if (e.getSource().equals(blockDiv)) {
            print = true;
            blockPrint = "blockDiv";
            System.out.println("blockDiv clicked.");
        } else if (e.getSource().equals(blockPow)) {
            print = true;
            blockPrint = "blockPow";
            System.out.println("blockPow clicked.");
        } else if (e.getSource().equals(connectBlocks)) {
            System.out.println("connectBlocks clicked.");
            connectFlag = true;
        } else if (e.getSource().equals(Run)) {
            System.out.println("Run clicked.");

            outputText.setEditable(true);
            outputText.setWrapText(true);

            //String out = readFile.runExecution(matrix);
            //System.out.println("OUT in Controller: " + out);
            String out = readFile.runExecution(inputMatrix, maxCol, maxRow);
            outputText.setText(out);
            outputText.setEditable(false);

        } else if (e.getSource().equals(Clean)) {
            System.out.println("Clean clicked.");
            cleanGrid();
        } else if(e.getSource().equals(delete)){
            System.out.println("Delete clicked.");
            deleteFlag = true;
        }
    }

    public void cleanGrid(){
        /* Clean gridPane */
        printGrid.getChildren().retainAll(printGrid.getChildren().get(0));
        inputMatrix = new AbstractBlock[maxRow*maxCol];
        imageMatrix = new ImageView[maxRow*maxCol];
        print = false;
        deleteFlag = false;
        complexFlag = false;
        connectFlag = false;
        loadFlag = false;
    }

    public void saveToFile(PrintWriter writer){
        String name = "";
        writer.println("7 9");
        for(int i = 0; i < maxCol; i++){
            for(int j = 0; j < maxRow; j++){
                int index =  i * maxRow + j;
                if(inputMatrix[i*maxRow + j] instanceof AbstractBlock) {
                    //matrix[i*rows + j].execute();
                    if (inputMatrix[index].getOutput().getType().equals("type1")) {

                        if (inputMatrix[index].getOutput() != null) {

                            name = inputMatrix[index].getNameBlock();

                            if (name.equals("blockValue")) {
                                writer.println("val " + inputMatrix[index].getOutput().getConnID() + " type1 " + inputMatrix[index].getInput1().getValue() + " none");
                            } else if (name.equals("blockInc") || name.equals("blockDec") || name.equals("blockNot")) {
                                writer.println(name + " " + inputMatrix[index].getOutput().getConnID() + " type1 " + inputMatrix[index].getInput1().getConnID() + " none");
                            } else {
                                if (name.equals("blockAdd")) {
                                    writer.println(name + " " + inputMatrix[index].getOutput().getConnID() + " type1 " + inputMatrix[index].getInput1().getConnID()+" "+((blockAdd)inputMatrix[index]).getInput2().getConnID());

                                } else if (name.equals("blockDiv")) {
                                    writer.println(name + " " + inputMatrix[index].getOutput().getConnID() + " type1 " + inputMatrix[index].getInput1().getConnID()+" "+((blockDiv)inputMatrix[index]).getInput2().getConnID());
                                } else if (name.equals("blockMul")) {
                                    writer.println(name + " " + inputMatrix[index].getOutput().getConnID() + " type1 " + inputMatrix[index].getInput1().getConnID()+" "+((blockMul)inputMatrix[index]).getInput2().getConnID());
                                } else if (name.equals("blockPow")) {
                                    writer.println(name + " " + inputMatrix[index].getOutput().getConnID() + " type1 " + inputMatrix[index].getInput1().getConnID()+" "+((blockPow)inputMatrix[index]).getInput2().getConnID());
                                } else if (name.equals("blockSub")) {
                                    writer.println(name + " " + inputMatrix[index].getOutput().getConnID() + " type1 " + inputMatrix[index].getInput1().getConnID()+" "+((blockSub)inputMatrix[index]).getInput2().getConnID());
                                }

                            }
                        }
                    }
                    else if(inputMatrix[index].getOutput().getType().equals("complex")) {
                        if (inputMatrix[index].getOutput() != null) {

                            name = inputMatrix[index].getNameBlock();

                            if (name.equals("blockValue")) {
                                writer.println("val " + inputMatrix[index].getOutput().getConnID() + " complex " + inputMatrix[index].getInput1().getValue() + " none");
                            } else if (name.equals("blockInc") || name.equals("blockDec") || name.equals("blockNot")) {
                                writer.println(name + " " + inputMatrix[index].getOutput().getConnID() + " complex " + inputMatrix[index].getInput1().getConnID() + " none");
                            } else {
                                if (name.equals("blockAdd")) {
                                    writer.println(name + " " + inputMatrix[index].getOutput().getConnID() + " complex " + inputMatrix[index].getInput1().getConnID()+" "+((blockAdd)inputMatrix[index]).getInput2().getConnID());

                                } else if (name.equals("blockDiv")) {
                                    writer.println(name + " " + inputMatrix[index].getOutput().getConnID() + " complex " + inputMatrix[index].getInput1().getConnID()+" "+((blockDiv)inputMatrix[index]).getInput2().getConnID());
                                } else if (name.equals("blockMul")) {
                                    writer.println(name + " " + inputMatrix[index].getOutput().getConnID() + " complex " + inputMatrix[index].getInput1().getConnID()+" "+((blockMul)inputMatrix[index]).getInput2().getConnID());
                                } else if (name.equals("blockPow")) {
                                    writer.println(name + " " + inputMatrix[index].getOutput().getConnID() + " complex " + inputMatrix[index].getInput1().getConnID()+" "+((blockPow)inputMatrix[index]).getInput2().getConnID());
                                } else if (name.equals("blockSub")) {
                                    writer.println(name + " " + inputMatrix[index].getOutput().getConnID() + " complex " + inputMatrix[index].getInput1().getConnID()+" "+((blockSub)inputMatrix[index]).getInput2().getConnID());
                                }

                            }
                        }
                    }
                }
                else{
                    writer.println("none");
                }
            }
        }

        writer.close();
    }

    @FXML
    public void onMenuItemClicked(ActionEvent e) throws Exception {

        if (e.getSource().equals(Help)) {
            System.out.println("Help clicked.");
        }
        else if (e.getSource().equals(Save)) {
            PrintWriter writer;

            System.out.println("Save clicked.");

            FileChooser fileChooser = new FileChooser();
            fileChooser.setTitle("Save Schema");
            File file = fileChooser.showSaveDialog(null);
            if (file != null) {
                try {
                    writer = new PrintWriter(file.getAbsoluteFile(), "UTF-8");
                    saveToFile(writer);

                  //  writer.println("The second line");
                   // writer.close();
                } catch (IOException ex) {
                    System.out.println(ex.getMessage());
                }
            }
        }
        else if (e.getSource().equals(Load)) {
            System.out.println("Load clicked.");

            cleanGrid();


            loadFlag = true;

            FileChooser fileChooser = new FileChooser();
            File selectedFile = fileChooser.showOpenDialog(null);
            if (selectedFile != null) {
                System.out.println("File selected: " + selectedFile.getAbsolutePath());
            }

            matrix = readFile.createMatrixFromFile(selectedFile.getAbsolutePath());

            for (int i = 0; i < readFile.cols; i++) {
                for (int j = 0; j < readFile.rows; j++) {
                    if (matrix[i * readFile.rows + j] instanceof AbstractBlock) {
                        printBlock(matrix[i * readFile.rows + j].getNameBlock(), i, j);

                        String name = matrix[i * readFile.rows + j].getOutput().getNameBlock();
                        if(!name.equals("blockValue")){
                            if (name.equals("blockInc") || name.equals("blockDec") || name.equals("blockNot")) {
                                //All blocks have at least one input, then if blocks with two inputs - call printLine() for their second inputs
                                int id1 = matrix[i * readFile.rows + j].getInput1().getConnID();
                                col1 = id1 / maxRow;    row1 = id1 % maxRow;
                                col2 = i;               row2 = j;
                                printLine();
                            }
                            if (name.equals("blockAdd")) {
                                int id1 = matrix[i * readFile.rows + j].getInput1().getConnID();
                                col1 = id1 / maxRow;    row1 = id1 % maxRow;
                                col2 = i;               row2 = j;
                                printLine();

                                int id2 = ((blockAdd) matrix[i * readFile.rows + j]).getInput2().getConnID();
                                col1 = id2/maxRow;
                                row1 = id2%maxRow;
                                col2 = i;
                                row2 = j;
                                printLine();
                            } else if (name.equals("blockDiv")) {
                                int id1 = matrix[i * readFile.rows + j].getInput1().getConnID();
                                col1 = id1 / maxRow;    row1 = id1 % maxRow;
                                col2 = i;               row2 = j;
                                printLine();

                                int id2 = ((blockDiv) matrix[i * readFile.rows + j]).getInput2().getConnID();
                                col1 = id2/maxRow;
                                row1 = id2%maxRow;
                                col2 = i;
                                row2 = j;
                                printLine();
                            } else if (name.equals("blockMul")) {
                                int id1 = matrix[i * readFile.rows + j].getInput1().getConnID();
                                col1 = id1 / maxRow;    row1 = id1 % maxRow;
                                col2 = i;               row2 = j;
                                printLine();

                                int id2 = ((blockMul) matrix[i * readFile.rows + j]).getInput2().getConnID();
                                col1 = id2/maxRow;
                                row1 = id2%maxRow;
                                col2 = i;
                                row2 = j;
                                printLine();
                            } else if (name.equals("blockPow")) {
                                int id1 = matrix[i * readFile.rows + j].getInput1().getConnID();
                                col1 = id1 / maxRow;    row1 = id1 % maxRow;
                                col2 = i;               row2 = j;
                                printLine();

                                int id2 = ((blockPow) matrix[i * readFile.rows + j]).getInput2().getConnID();
                                col1 = id2/maxRow;
                                row1 = id2%maxRow;
                                col2 = i;
                                row2 = j;
                                printLine();
                            } else if (name.equals("blockSub")) {
                                int id1 = matrix[i * readFile.rows + j].getInput1().getConnID();
                                col1 = id1 / maxRow;    row1 = id1 % maxRow;
                                col2 = i;               row2 = j;
                                printLine();

                                int id2 = ((blockSub) matrix[i * readFile.rows + j]).getInput2().getConnID();
                                col1 = id2/maxRow;
                                row1 = id2%maxRow;
                                col2 = i;
                                row2 = j;
                                printLine();
                            }
                        }
                    }
                }
            }
            loadFlag = false;
        }

    }

    boolean flag1 = true;


    public void printLine(){
        int index = col1*maxRow + row1;
    //    Tooltip t = new Tooltip(result.toString());

        /* Connect blocks */
        connectBlocks();

        /* Set information about output of every block - will display when cursor will be on line */
        result.delete(0, result.length());
        result.append(inputMatrix[index].getOutput().getNameBlock());
        result.append(newLine);
        result.append("ID: ");
        result.append(inputMatrix[index].getOutput().getConnID());
        result.append(newLine);
        result.append("Output: ");

        if(complexFlag) {
            result.append(inputMatrix[index].getOutput().getComplex());
            System.out.println("COMPLEX");
        }
        else{
            result.append(inputMatrix[index].getOutput().getValue());
        }



        if(row1 == row2) {
            Line line = new Line();
            line.setStartX(0.0);
            line.setStartY(0.0);
            line.setEndX(20.0);
            line.setEndY(0.0);
            printGrid.setConstraints(line, col1, row1);
            printGrid.setHalignment(line, HPos.RIGHT);

            Line line2 = new Line();
            line2.setStartX(10.0);
            line2.setStartY(0.0);
            line2.setEndX(30.0);
            line2.setEndY(0.0);
            printGrid.setHalignment(line2, HPos.LEFT);
            printGrid.setConstraints(line2, col2, row2);

            printGrid.getChildren().addAll(line, line2);

            Tooltip t = new Tooltip(result.toString());
            Tooltip tline = new Tooltip(result.toString());
            Tooltip.install(imageMatrix[index], t);
            Tooltip.install(line, tline);
        }
        else if(row1 > row2){
            Line line = new Line();
            line.setStartX(20.0);
            line.setStartY(-50.0);
            line.setEndX(0.0);
            line.setEndY(0.0);
            printGrid.setConstraints(line, col1, row1);
            printGrid.setHalignment(line, HPos.RIGHT);
            printGrid.setValignment(line, VPos.TOP);

            Line line2 = new Line();
            line2.setStartX(20.0);
            line2.setStartY(-50.0);
            line2.setEndX(0.0);
            line2.setEndY(0.0);
            printGrid.setHalignment(line2, HPos.LEFT);
            printGrid.setValignment(line2, VPos.BOTTOM);
            printGrid.setConstraints(line2, col2, row2);
            printGrid.getChildren().addAll(line, line2);

            Tooltip t = new Tooltip(result.toString());
            Tooltip tline = new Tooltip(result.toString());
            Tooltip.install(imageMatrix[index], t);
            Tooltip.install(line, tline);
        }
        else if(row1 < row2){
            Line line = new Line();
            line.setStartX(20.0);
            line.setStartY(50.0);
            line.setEndX(0.0);
            line.setEndY(0.0);
            printGrid.setConstraints(line, col1, row1);
            printGrid.setHalignment(line, HPos.RIGHT);
            printGrid.setValignment(line, VPos.BOTTOM);

            Line line2 = new Line();
            line2.setStartX(20.0);
            line2.setStartY(50.0);
            line2.setEndX(0.0);
            line2.setEndY(0.0);
            printGrid.setHalignment(line2, HPos.LEFT);
            printGrid.setValignment(line2, VPos.TOP);
            printGrid.setConstraints(line2, col2, row2);
            printGrid.getChildren().addAll(line, line2);

            Tooltip t = new Tooltip(result.toString());
            Tooltip tline = new Tooltip(result.toString());
            Tooltip.install(imageMatrix[index], t);
            Tooltip.install(line, tline);
        }

    }

    /**
     *  Function will connect two blocks.
     *  */
    public void connectBlocks(){
        /* Propojeni bloku - ulozeni vystupu jednoho bloku do druheho */
        int index1 = col1 * maxRow + row1;
        int index2 = col2 * maxRow + row2;
        String nameSecond = inputMatrix[index2].getOutput().getNameBlock();

        if(nameSecond.equals("blockNot") || nameSecond.equals("blockInc") || nameSecond.equals("blockDec")) {
            inputMatrix[index2].setInput1(inputMatrix[index1].getOutput());
            System.out.println("\n[INBlock_1]: " + inputMatrix[index2].getInput1().getNameBlock() + "\n[INValue_1]: " + inputMatrix[index2].getInput1().getValue());

            inputMatrix[index2].execute();
            System.out.println("Test execute - result: "+inputMatrix[index2].getOutput().getValue());
        }
        else if(nameSecond.equals("blockAdd")){
            if(inputMatrix[index2].getInput1() != null){
                ((blockAdd) inputMatrix[index2]).setInput2(inputMatrix[index1].getOutput());
                System.out.println("\n[INBlock_2]: " + ((blockAdd) inputMatrix[index2]).getInput2().getNameBlock()+" -- [INBlock ID]: "+((blockAdd) inputMatrix[index2]).getInput2().getConnID());

                inputMatrix[index2].execute();
                System.out.println("Test execute - result: "+inputMatrix[index2].getOutput().getValue());
            }
            else{
                inputMatrix[index2].setInput1(inputMatrix[index1].getOutput());
                System.out.println("\n[INBlock_1]: " + inputMatrix[index2].getInput1().getNameBlock()+" -- [INBlock ID]: "+inputMatrix[index2].getInput1().getConnID());
            }
        }
        else if(nameSecond.equals("blockDiv")){
            if(inputMatrix[index2].getInput1() != null){
                ((blockDiv) inputMatrix[index2]).setInput2(inputMatrix[index1].getOutput());
                System.out.println("\n[INBlock_2]: " + ((blockDiv) inputMatrix[index2]).getInput2().getNameBlock()+" -- [INBlock ID]: "+((blockDiv) inputMatrix[index2]).getInput2().getConnID());

                inputMatrix[index2].execute();
                System.out.println("Test execute - result: "+inputMatrix[index2].getOutput().getValue());
            }
            else{
                inputMatrix[index2].setInput1(inputMatrix[index1].getOutput());
                System.out.println("\n[INBlock_1]: " + inputMatrix[index2].getInput1().getNameBlock()+" -- [INBlock ID]: "+inputMatrix[index2].getInput1().getConnID());
            }
        }
        else if(nameSecond.equals("blockMul")){
            if(inputMatrix[index2].getInput1() != null){
                ((blockMul) inputMatrix[index2]).setInput2(inputMatrix[index1].getOutput());
                System.out.println("\n[INBlock_2]: " + ((blockMul) inputMatrix[index2]).getInput2().getNameBlock()+" -- [INBlock ID]: "+((blockMul) inputMatrix[index2]).getInput2().getConnID());

                inputMatrix[index2].execute();
                System.out.println("Test execute - result: "+inputMatrix[index2].getOutput().getValue());
            }
            else{
                inputMatrix[index2].setInput1(inputMatrix[index1].getOutput());
                System.out.println("\n[INBlock_1]: " + inputMatrix[index2].getInput1().getNameBlock()+" -- [INBlock ID]: "+inputMatrix[index2].getInput1().getConnID());
            }
        }
        else if(nameSecond.equals("blockPow")){
            if(inputMatrix[index2].getInput1() != null){
                ((blockPow) inputMatrix[index2]).setInput2(inputMatrix[index1].getOutput());
                System.out.println("\n[INBlock_2]: " + ((blockPow) inputMatrix[index2]).getInput2().getNameBlock()+" -- [INBlock ID]: "+((blockPow) inputMatrix[index2]).getInput2().getConnID());

                inputMatrix[index2].execute();
                System.out.println("Test execute - result: "+inputMatrix[index2].getOutput().getValue());
            }
            else{
                inputMatrix[index2].setInput1(inputMatrix[index1].getOutput());
                System.out.println("\n[INBlock_1]: " + inputMatrix[index2].getInput1().getNameBlock()+" -- [INBlock ID]: "+inputMatrix[index2].getInput1().getConnID());
            }
        }
        else if(nameSecond.equals("blockSub")){
            if(inputMatrix[index2].getInput1() != null){
                ((blockSub) inputMatrix[index2]).setInput2(inputMatrix[index1].getOutput());
                System.out.println("\n[INBlock_2]: " + ((blockSub) inputMatrix[index2]).getInput2().getNameBlock()+" -- [INBlock ID]: "+((blockSub) inputMatrix[index2]).getInput2().getConnID());

                inputMatrix[index2].execute();
                System.out.println("Test execute - result: "+inputMatrix[index2].getOutput().getValue());
            }
            else{
                inputMatrix[index2].setInput1(inputMatrix[index1].getOutput());
                System.out.println("\n[INBlock_1]: " + inputMatrix[index2].getInput1().getNameBlock()+" -- [INBlock ID]: "+inputMatrix[index2].getInput1().getConnID());
            }
        }
    }


    /* Udela print block na tom indexu gridu, kde klikne uzivatel -- rozmisti blok v CENTRU bunky (dodelat) */
    @FXML
    public void onMouseRelease(MouseEvent event) {
        System.out.println("onMouseRelease - in clicked position will be printed block");
        System.out.println("X: " + event.getX() + ", Y: " + event.getY());

        /* Get grid pane index od row and column */
        int column = (int) event.getX() / 100;
        int row = (int) event.getY() / 100;

        System.out.println("Index of grid: [" + column + "][" + row + "]");

        if(imageMatrix[column*maxRow + row] instanceof ImageView) {
            System.out.println("Here is some block");

            /* ---------------------- Propojeni bloku -------------------------*/
            if (connectFlag) {
                countConn += 1;
                if (countConn == 1) {
                    col1 = column;
                    row1 = row;
                }
                else if (countConn == 2) {
                    col2 = column;
                    row2 = row;
                    printLine();


                    countConn = 0;
                }
                //System.out.println("Here is some block: "+inputMatrix[column*maxRow + row].getOutput().getNameBlock());
            }
        }


        /* Je-li stisknuto tlacitko s typem blokem a kliknuto myskou na bunku v gridu - zavolat tisk prislusneho bloku */
        if (print) {
            if (blockPrint.equals("blockValue")) {
                setValueFromTextfield();
                printBlock("blockValue", column, row);
                print = false;
                connectFlag = false; //kdyz po kliknuti na connect se neklikne na vstupni a vystupni blok na platne
                countConn = 0;
            } else if (blockPrint.equals("blockNot")) {
                printBlock("blockNot", column, row);
                print = false;
                connectFlag = false;
                countConn = 0;
            } else if (blockPrint.equals("blockInc")) {
                printBlock("blockInc", column, row);
                print = false;
                connectFlag = false;
                countConn = 0;
            } else if (blockPrint.equals("blockDec")) {
                printBlock("blockDec", column, row);
                print = false;
                connectFlag = false;
                countConn = 0;
            } else if (blockPrint.equals("blockAdd")) {
                printBlock("blockAdd", column, row);
                print = false;
                connectFlag = false;
                countConn = 0;
            } else if (blockPrint.equals("blockSub")) {
                printBlock("blockSub", column, row);
                print = false;
                connectFlag = false;
                countConn = 0;
            } else if (blockPrint.equals("blockMul")) {
                printBlock("blockMul", column, row);
                print = false;
                connectFlag = false;
                countConn = 0;
            } else if (blockPrint.equals("blockDiv")) {
                printBlock("blockDiv", column, row);
                print = false;
                connectFlag = false;
                countConn = 0;
            } else if (blockPrint.equals("blockPow")) {
                printBlock("blockPow", column, row);
                print = false;
                connectFlag = false;
                countConn = 0;
            } else if (blockPrint.equals("connectBlocks")) {
                System.out.println("Waiting for connection...");
            }
        }
        else if(deleteFlag){
            deleteFlag = false;
            connectFlag = false;
            printGrid.getChildren().remove(imageMatrix[column*maxRow + row]);
        }
    }


    public void printBlock(String name, int column, int row) {
        int index = column*maxRow+row;
        String type = "";


        if (name.equals("blockValue")) {
            Image image = new Image("sample/resources/VAL.png");
            ImageView i = new ImageView(image);


            imageMatrix[index] = i;
            printGrid.setConstraints(imageMatrix[index], column, row);
            printGrid.setHalignment(imageMatrix[index], HPos.CENTER);
            printGrid.getChildren().add(imageMatrix[index]);



            if(loadFlag){
                if(matrix[index].getOutput().getType().equals("complex")){              //je-li blockVal z souboru typu complex
                    inputMatrix[index] = new blockValue("complex", matrix[index].getOutput().getComplex(), index);
                    complexFlag = true;
                }
                else{
                    inputMatrix[index] = new blockValue("type1", matrix[index].getOutput().getValue(), index);
                }
            }else {
                /* Vstupy zadava uzivatel */
                if (complexFlag) {
                    inputMatrix[index] = new blockValue("complex", inputComplex, index);

                } else {
                    inputMatrix[index] = new blockValue("type1", inputDouble, index);
                }
            }
            inputMatrix[index].setNameBlock("blockValue");
            inputMatrix[index].getOutput().setNameBlock("blockValue");

            System.out.println("\n --- INPUT is of type: " + inputMatrix[index].getOutput().getType() + " -------");
            System.out.println(" --- INPUT value: " + inputMatrix[index].getOutput().getValue() + " -------");     //getValue() for double and getComplex() for complex
            System.out.println(" --- INPUT ID: " + inputMatrix[index].getOutput().getConnID() + " -------");
            System.out.println(" --- INPUT Block name: " + inputMatrix[index].getOutput().getNameBlock() + " -------\n");

        }
        else if (name.equals("blockNot")) {
            Image image = new Image("sample/resources/NOT.png");
            ImageView i = new ImageView(image);

            imageMatrix[index] = i;
            printGrid.setConstraints(imageMatrix[index], column, row);
            printGrid.setHalignment(imageMatrix[index], HPos.CENTER);
            printGrid.getChildren().add(imageMatrix[index]);

            if(complexFlag) {
                inputMatrix[index] = new blockNot("complex", index);

            }else{
                inputMatrix[index] = new blockNot("type1", index);
            }
            inputMatrix[index].setNameBlock("blockNot");
            inputMatrix[index].getOutput().setNameBlock("blockNot");
            System.out.println("\n --- INPUT ID: " + inputMatrix[index].getOutput().getConnID() + " -------");
            System.out.println(" --- INPUT Block name: " + inputMatrix[index].getOutput().getNameBlock() + " -------");
        }
        else if (name.equals("blockInc")) {
            Image image = new Image("sample/resources/INC.png");
            ImageView i = new ImageView(image);

            imageMatrix[index] = i;
            printGrid.setConstraints(imageMatrix[index], column, row);
            printGrid.setHalignment(imageMatrix[index], HPos.CENTER);
            printGrid.getChildren().add(imageMatrix[index]);

            if(complexFlag) {
                inputMatrix[index] = new blockInc("complex", index);

            }else{
                inputMatrix[index] = new blockInc("type1", index);
            }
            inputMatrix[index].setNameBlock("blockInc");
            inputMatrix[index].getOutput().setNameBlock("blockInc");
            System.out.println("\n --- INPUT ID: " + inputMatrix[index].getOutput().getConnID() + " -------");
            System.out.println(" --- INPUT Block name: " + inputMatrix[index].getOutput().getNameBlock() + " -------");
        }
        else if (name.equals("blockDec")) {
            Image image = new Image("sample/resources/DEC.png");
            ImageView i = new ImageView(image);

            imageMatrix[index] = i;
            printGrid.setConstraints(imageMatrix[index], column, row);
            printGrid.setHalignment(imageMatrix[index], HPos.CENTER);
            printGrid.getChildren().add(imageMatrix[index]);

            if(complexFlag) {
                inputMatrix[index] = new blockDec("complex", index);

            }else{
                inputMatrix[index] = new blockDec("type1", index);
            }
            inputMatrix[index].setNameBlock("blockDec");
            inputMatrix[index].getOutput().setNameBlock("blockDec");
            System.out.println("\n --- INPUT ID: " + inputMatrix[index].getOutput().getConnID() + " -------");
            System.out.println(" --- INPUT Block name: " + inputMatrix[index].getOutput().getNameBlock() + " -------");
        }
        else if (name.equals("blockAdd")) {
            Image image = new Image("sample/resources/ADD2.png");
            ImageView i = new ImageView(image);

            imageMatrix[index] = i;
            printGrid.setConstraints(imageMatrix[index], column, row);
            printGrid.setHalignment(imageMatrix[index], HPos.CENTER);
            printGrid.getChildren().add(imageMatrix[index]);

            if(complexFlag) {
                inputMatrix[index] = new blockAdd("complex", index);

            }else{
                inputMatrix[index] = new blockAdd("type1", index);
            }
            inputMatrix[index].setNameBlock("blockAdd");
            inputMatrix[index].getOutput().setNameBlock("blockAdd");
            System.out.println("\n --- INPUT ID: " + inputMatrix[index].getOutput().getConnID() + " -------");
            System.out.println(" --- INPUT Block name: " + inputMatrix[index].getOutput().getNameBlock() + " -------");
        }
        else if (name.equals("blockSub")) {
            Image image = new Image("sample/resources/SUB.png");
            ImageView i = new ImageView(image);

            imageMatrix[index] = i;
            printGrid.setConstraints(imageMatrix[index], column, row);
            printGrid.setHalignment(imageMatrix[index], HPos.CENTER);
            printGrid.getChildren().add(imageMatrix[index]);

            if(complexFlag) {
                inputMatrix[index] = new blockSub("complex", index);

            }else{
                inputMatrix[index] = new blockSub("type1", index);
            }
            inputMatrix[index].setNameBlock("blockSub");
            inputMatrix[index].getOutput().setNameBlock("blockSub");
            System.out.println("\n --- INPUT ID: " + inputMatrix[index].getOutput().getConnID() + " -------");
            System.out.println(" --- INPUT Block name: " + inputMatrix[index].getOutput().getNameBlock() + " -------");
        }
        else if (name.equals("blockMul")) {
            Image image = new Image("sample/resources/MUL.png");
            ImageView i = new ImageView(image);

            imageMatrix[index] = i;
            printGrid.setConstraints(imageMatrix[index], column, row);
            printGrid.setHalignment(imageMatrix[index], HPos.CENTER);
            printGrid.getChildren().add(imageMatrix[index]);

            if(complexFlag) {
                inputMatrix[index] = new blockMul("complex", index);

            }else{
                inputMatrix[index] = new blockMul("type1", index);
            }
            inputMatrix[index].setNameBlock("blockMul");
            inputMatrix[index].getOutput().setNameBlock("blockMul");
            System.out.println("\n --- INPUT ID: " + inputMatrix[index].getOutput().getConnID() + " -------");
            System.out.println(" --- INPUT Block name: " + inputMatrix[index].getOutput().getNameBlock() + " -------");
        }
        else if (name.equals("blockDiv")) {
            Image image = new Image("sample/resources/DIV.png");
            ImageView i = new ImageView(image);

            imageMatrix[index] = i;
            printGrid.setConstraints(imageMatrix[index], column, row);
            printGrid.setHalignment(imageMatrix[index], HPos.CENTER);
            printGrid.getChildren().add(imageMatrix[index]);

            if(complexFlag) {
                inputMatrix[index] = new blockDiv("complex", index);

            }else{
                inputMatrix[index] = new blockDiv("type1", index);
            }
            inputMatrix[index].setNameBlock("blockDiv");
            inputMatrix[index].getOutput().setNameBlock("blockDiv");
            System.out.println("\n --- INPUT ID: " + inputMatrix[index].getOutput().getConnID() + " -------");
            System.out.println(" --- INPUT Block name: " + inputMatrix[index].getOutput().getNameBlock() + " -------");
        }
        else if (name.equals("blockPow")) {
            Image image = new Image("sample/resources/POW.png");
            ImageView i = new ImageView(image);

            imageMatrix[index] = i;
            printGrid.setConstraints(imageMatrix[index], column, row);
            printGrid.setHalignment(imageMatrix[index], HPos.CENTER);
            printGrid.getChildren().add(imageMatrix[index]);

            if(complexFlag) {
                inputMatrix[index] = new blockPow("complex", index);

            }else{
                inputMatrix[index] = new blockPow("type1", index);
            }
            inputMatrix[index].setNameBlock("blockPow");
            inputMatrix[index].getOutput().setNameBlock("blockPow");
            System.out.println("\n --- INPUT ID: " + inputMatrix[index].getOutput().getConnID() + " -------");
            System.out.println(" --- INPUT Block name: " + inputMatrix[index].getOutput().getNameBlock() + " -------");
        }

    }


}