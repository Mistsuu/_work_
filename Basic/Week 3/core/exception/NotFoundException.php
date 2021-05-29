<?php

class NotFoundException extends Exception
{
    protected $message = 'Bạn nàm gì đóa? Nơi đây không có gì đâu.';
    protected $code    = 404;
};