<?php

class AdminMiddleware extends BaseMiddleware
{

    public array $actions = [];

    public function __construct(array $actions=[])
    {
        $this->actions = $actions;
    }

    public function execute()
    {
        if (Application::$app->session->isGuest() || Application::$app->session->isRegularUser()) {
            if (empty($this->actions) || in_array(Application::$app->controller->action, $this->actions)) {
                throw new ForbiddenException();
            }
        }
    }

};