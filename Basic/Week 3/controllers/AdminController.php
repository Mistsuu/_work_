<?php

class AdminController extends Controller
{

    public function __construct()
    {
        $this->registerMiddleware(new AdminMiddleware([]));
    }

    public function editStuffs(Request $request, Response $response, Session $session)
    {
        return $this->render('editStuffs');
    }

    public function editUsers(Request $request, Response $response, Session $session)
    {
        return $this->render('editUsers');
    }

};