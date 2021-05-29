<?php

class ForbiddenException extends Exception
{
    protected $message = 'Xin lỗi nhưng bạn đang xâm phạm một lãnh thổ không cho phép bạn xâm phạm nếu bạn không có quyền xâm phạm.';
    protected $code    = 403;
};