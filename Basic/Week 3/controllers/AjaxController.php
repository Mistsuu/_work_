<?php

class AjaxController extends Controller
{

    public function __construct()
    {
        $this->registerMiddleware(new AuthMiddleware(['addToCart', 'removeFromCart', 'getUsername', 'getCartIDs', 'viewCart', 'getUser']));
        $this->registerMiddleware(new AdminMiddleware(['getAllUsers', 'modifyUser', 'removeUser', 'removeStuff', 'modifyStuff', 'addStuff', 'addUser']));
    }

    public function addToCart(Request $request, Response $response, Session $session)
    {
        // Only success if we get the data
        if ($request->getBody()['itemID']) {
            // Add cart ID to the $_SESSION['cart_ids'] array
            $cart_ids   = $session->get('cart_ids') ? $session->get('cart_ids') : [];
            $cart_ids[] = $request->getBody()['itemID'];
            $session->set('cart_ids', $cart_ids);
            
            // Set flash message
            $session->setFlash('success', 'Thêm vào giỏ hàng thành công!');

            return;
        }

        $session->setFlash('danger', 'Thêm vào giỏ hàng không thành công!');
        return json_encode($_POST);
    }

    public function removeFromCart(Request $request, Response $response, Session $session)
    {
        // Remove cart ID from the $_SESSION['cart_ids'] array
        $cart_ids = $session->get('cart_ids') ? $session->get('cart_ids') : [];
        array_splice($cart_ids, $request->getBody()['itemIndex'], 1);
        $session->set('cart_ids', $cart_ids);

        // Set flash message
        $session->setFlash('success', 'Item bay màu khỏi giỏ hàng thành công!');
    }

    public function getUserStatus(Request $request, Response $response, Session $session)
    {
        if ($session->isGuest())       return 'guest';
        if ($session->isRegularUser()) return 'user';
        return 'admin';
    }

    public function getUsername(Request $request, Response $response, Session $session)
    {
        if (!$session->isGuest()) {
            $userID = $session->get('user');
            $user   = User::findOne([User::getPrimaryKey() => $userID]);
            return $user->getDisplayName();
        }
    }

    public function getFlashMessage(Request $request, Response $response, Session $session)
    {
        return $session->popFlash($request->getBody()['flashName']);
    }

    public function getCartIDs(Request $request, Response $response, Session $session)
    {
        return json_encode($session->get('cart_ids') ? $session->get('cart_ids') : []);
    }

    public function getStuffByID(Request $request, Response $response, Session $session)
    {
        $stuff = Stuff::findOne([Stuff::getPrimaryKey() => $request->getBody()['itemID']]);
        return json_encode($stuff);
    }

    public function getAllStuffs(Request $request, Response $response, Session $session)
    {
        $stuffs = Stuff::findAll();
        return json_encode($stuffs);
    }

    public function getUser(Request $request, Response $response, Session $session)
    {
        // Find user
        $user = User::findOne([User::getPrimaryKey() => $session->get('user')]);

        // Unset these values so that we don't send it to the client by default :P
        unset($user->password);
        unset($user->passwordConfirm);

        // Return JSON encoded data
        return json_encode($user);
    }

    public function getAllUsers(Request $request, Response $response, Session $session)
    {
        $users = User::findAll();
        return json_encode($users);
    }

    public function modifyUser(Request $request, Response $response, Session $session)
    {
        // Decode JSON data
        $userData = json_decode(unescapeJSON($request->getBody()['newUser']));
        
        // Load data into variable
        $editUserForm = new EditUserForm();
        $editUserForm->loadData($userData);

        // Validate and confirm
        if (!$editUserForm->validate() || !$editUserForm->update())
            return json_encode([
                "status" => "failure",
                "errors" => $editUserForm->errors,
                "user"   => User::findOne([User::getPrimaryKey() => $editUserForm->{User::getPrimaryKey()}])
            ]);

        return json_encode([
            "status" => "success",
            "user"   => User::findOne([User::getPrimaryKey() => $editUserForm->{User::getPrimaryKey()}])
        ]);
    }

    public function removeUser(Request $request, Response $response, Session $session)
    {
        $userID = $request->getBody()['userID'];
        User::removeOne([User::getPrimaryKey() => $userID]);
        return json_encode([
            "status" => "success"
        ]);
    }

    public function addUser(Request $request, Response $response, Session $session)
    {
        // Decode JSON data
        $userData = json_decode(unescapeJSON($request->getBody()['newUser']));
        
        // Load data into variable
        $addUserForm = new AddUserForm();
        $addUserForm->loadData($userData);

        // Validate and confirm
        if (!$addUserForm->validate() || !$addUserForm->add())
            return json_encode([
                "status" => "failure",
                "errors" => $addUserForm->errors
            ]);

        return json_encode([
            "status" => "success",
            "user"   => User::findOne(['email' => $addUserForm->email])
        ]);
    }

    public function modifyStuff(Request $request, Response $response, Session $session)
    {
        // Decode JSON data
        $stuffData = json_decode(unescapeJSON($request->getBody()['newStuff']));
        
        // Load data into variable
        $editStuffForm = new EditStuffForm();
        $editStuffForm->loadData($stuffData);

        // var_dump($editStuffForm);

        // Validate and confirm
        if (!$editStuffForm->validate() || !$editStuffForm->update())
            return json_encode([
                "status" => "failure",
                "errors" => $editStuffForm->errors,
                "stuff"  => Stuff::findOne([Stuff::getPrimaryKey() => $editStuffForm->{Stuff::getPrimaryKey()}])
            ]);

        return json_encode([
            "status" => "success",
            "stuff"  => Stuff::findOne([Stuff::getPrimaryKey() => $editStuffForm->{Stuff::getPrimaryKey()}])
        ]);
    }

    public function addStuff(Request $request, Response $response, Session $session)
    {
        // Decode JSON data
        $stuffData = json_decode(unescapeJSON($request->getBody()['newStuff']));
        
        // Load data into variable
        $addStuffForm = new AddStuffForm();
        $addStuffForm->loadData($stuffData);

        // Validate and confirm
        if (!$addStuffForm->validate() || !$addStuffForm->add())
            return json_encode([
                "status" => "failure",
                "errors" => $addStuffForm->errors
            ]);

        return json_encode([
            "status" => "success",
            "stuff"  => Stuff::findOne(['name' => $addStuffForm->name])
        ]);
    }

    public function removeStuff(Request $request, Response $response, Session $session)
    {
        $stuffID = $request->getBody()['stuffID'];
        Stuff::removeOne([Stuff::getPrimaryKey() => $stuffID]);
        return json_encode([
            "status" => "success"
        ]);
    }

};