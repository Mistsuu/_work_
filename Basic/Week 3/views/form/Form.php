<?php

class Form
{

    public static function begin($action, $method)
    {
        echo sprintf('<form action="%s" method="%s">', $action, $method);
    }

    public static function end()
    {
        echo '</form>';
    }

};