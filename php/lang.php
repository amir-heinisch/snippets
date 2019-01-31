<?php

# Some php language syntax.

/* Secure script a bit. */
defined('APPLICATION_LOADED') OR die('No direct script access.');

/* This is a comment. */

/*
 * This is a longer comment.
 */

/* Define global const. */
define('myConst', TRUE);

/* Include other scripts. */
include('/file.php');
require('/file.php');

/* Get current directory. */
dirname(__FILE__);

/* Assign variable. */
$v = 12;

/* Create object. */
$mv = new MainView();

/* Call method on object. */
$mv->get_view();

/* Access GET parameter. */
$_GET['content'];

/* echo out some stuff. */
echo "hi";

/* Check if value exists and return it. */
echo $_GET['content'] ?? "was empty.";

/* array. */
$arr = array(
    'a' => 1,
    'b' => 2,
    'c' => 3
);

echo $arr['b'];

/* if statement */
if (TRUE) {
    return TRUE;
} else {
    return FALSE;
}

/* switch statement */
switch ($v) {
    case 1:
        echo '1';
        break;
    default:
        exit(1);
}

/* exit a script. */
exit(1);

