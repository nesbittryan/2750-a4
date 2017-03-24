<?php
$username = $_POST["username"];
$author = $_POST["author"];
$flag = $_POST["flag"];
$streamlist = $_POST["streamlist"];

exec("./addauthor $flag $author $streamlist 2>&1", $addauthor);

echo "Currently logged in as: $username";
exec("./sc header.wpml STREAM_NULL MSG_NULL $username 2>&1", $header);
foreach($header as $line) {
    echo "$line";
}
exec("./sc addremoveAuthor.wpml STREAM_NULL MSG_NULL $username 2>&1", $return);
foreach($return as $line) {
    echo "$line";
}
foreach($addauthor as $line) {
    echo "<br>$line";
}
?>
