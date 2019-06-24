package eu.mihosoft.nativefx;

import java.nio.ByteBuffer;

import javafx.application.Application;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.layout.Region;
import javafx.scene.layout.StackPane;
import javafx.stage.Stage;

public class NativeFXApp extends Application {

    public void start(Stage primaryStage) {

        NativeBinding.init();
        
        int key = NativeBinding.connectTo("_mem");

        Parent root = new NativeNode(key);

        root.setStyle("-fx-border-color: green;");

        Scene scene = new Scene(new StackPane(root), 1024,768);

        primaryStage.setTitle("NativeFX Test");
        primaryStage.setScene(scene);
        primaryStage.setOnCloseRequest((value)->System.exit(0));
        primaryStage.show();
    }

    public static void main(String[] args) {
        launch(args);
    }

}

