<?php
$username = $_POST['username'];
$streamChoice = $_POST['streamChoice'];

echo "Currently logged in as: $username";
exec("./sc header.wpml STREAM_NULL MSG_NULL $username", $header);
foreach($header as $line) {
    echo "$line";
}

?>
