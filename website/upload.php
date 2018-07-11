<?php

if(!empty($_FILES['uploaded_file'])) {
    $can_upload = True;

    $path = "uploads/";
    $path = $path . basename( $_FILES['uploaded_file']['name']);

    $file_type = strtolower(pathinfo($path, PATHINFO_EXTENSION));

    if ($file_type != "csv") {
        echo "File type not supported. Please upload a .csv file.";
        $can_upload = False;
    }

    if($can_upload && move_uploaded_file($_FILES['uploaded_file']['tmp_name'], $path)) {
        session_start();
        $_SESSION["file_path"] = $path;
        header('Location: pony_gp.php');
    } else {
        echo "There was an error uploading the file, please try again!";
    }
}

exit;
?>