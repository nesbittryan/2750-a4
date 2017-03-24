<?php
$username = $_POST['username'];

echo "Currently logged in as: $username";
exec("./sc header.wpml STREAM_NULL MSG_NULL $username", $header);
foreach($header as $line) {
    echo "$line";
}
exec("./view.py $username", $status);
foreach($status as $line) {
    echo "$line";
}
?>
