<?php
exec("./sc index.wpml", $index);
foreach($index as $line) {
    echo "$line";
}
?>
