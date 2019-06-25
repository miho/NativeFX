package eu.mihosoft.nativefx;

import java.nio.ByteBuffer;

import javafx.application.Application;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Region;
import javafx.scene.layout.StackPane;
import javafx.stage.Stage;

public class NativeFXApp extends Application {

    public void start(Stage primaryStage) {

        NativeNode root = new NativeNode();
        root.connect("_mem_1");

        root.setMinWidth(100);

        // NativeNode root2 = new NativeNode();
        // root2.connect("_mem_2");

        // root2.setMinWidth(700);

        // Scene scene = new Scene(new HBox(root, root2), 1024,768);

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

