<?php
exec("./sc index.wpml STREAM_NULL MSG_NULL NAME_NULL", $index);
foreach($index as $line) {
    echo "$line";
}
?>
