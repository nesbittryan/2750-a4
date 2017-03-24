<?php
$username = $_POST['username'];
$streamChoice = $_POST['streamChoice'];
$username = $_POST['username'];
$messageNum = $_POST['messageNum'];
$sorting = $_POST['sorting'];

echo "Currently logged in as: $username";
if((isset($streamChoice)) and ($streamChoice != "NULL_STREAM")) {
    echo ", in Stream: $streamChoice";
}
echo "<br>";
exec("./sc header.wpml STREAM_NULL MSG_NULL $username", $header);
foreach($header as $line) {
    echo "$line";
}

if((isset($streamChoice)) and ($streamChoice != "NULL_STREAM")) {
    exec("./sc home.wpml $streamChoice $messageNum $username", $return);
    foreach($return as $line) {
        echo "$line";
    }
    echo "<br>";
    exec("./view.py STREAM_NAME $streamChoice $messageNum $sorting $username", $displayMsg);
    foreach($displayMsg as $line) {
        echo "$line";
    }
} else {
    echo "Error in Posting<br>";
}
?>
