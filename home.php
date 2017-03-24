<?php
$username = $_POST['username'];
$streamChoice = $_POST['streamChoice'];
if($username == "") {
    header("Location:index.php");
    exit;
}
echo "Currently logged in as: $username";
if((isset($streamChoice)) and ($streamChoice != "NULL_STREAM")) {
    echo ", in Stream: $streamChoice";
}
exec("./sc header.wpml STREAM_NULL MSG_NULL $username 2>&1", $header);
foreach($header as $line) {
    echo "$line";
}
if((isset($streamChoice)) and ($streamChoice != "NULL_STREAM")) {
    exec("./sc home.wpml $streamChoice MSG_NULL $username 2>&1", $return);
    foreach($return as $line) {
        echo "$line";
    }
    echo "<br>";
    exec("./view.py STREAM_NAME $streamChoice -1 0 $username 2>&1", $displayMsg);
    foreach($displayMsg as $line) {
        echo "$line";
    }
} else {
    echo "Select a Stream to start viewing<br>";
}
?>
