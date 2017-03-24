<?php
$username = $_POST["username"];
$stream = $_POST["stream"];
$message = $_POST["message"];

exec("./post $stream $username MESSAGE_TEXT $message", $status);

echo "Currently logged in as: $username";
exec("./sc header.wpml STREAM_NULL MSG_NULL $username", $header);
foreach($header as $line) {
    echo "$line";
}
exec("./sc addPost.wpml STREAM_NULL MSG_NULL $username", $return);
foreach($return as $line) {
    echo "$line";
}
foreach($status as $line) {
    echo "<br>$line";
}
?>
