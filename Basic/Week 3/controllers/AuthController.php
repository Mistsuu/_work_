<?php

class AuthController extends Controller
{

    public function __construct()
    {
        $this->registerMiddleware(new AuthMiddleware(['profile', 'editProfile', 'viewCart', 'checkout']));
    }

    public function login(Request $request, Response $response, Session $session)
    {
        // If not guest, get the fork outta here...
        if (!$session->isGuest()) {
            $response->redirect('/');
            return;
        }

        $loginForm = new LoginForm();

        // Handles data if request is POST
        if ($request->isPost()) {
            // Load data
            $loginForm->loadData($request->getBody());

            // Check condition
            if ($loginForm->validate() && ($user = $loginForm->login())) {
                // Set session for user
                $session->set('user', $user->{$user->getPrimaryKey()});
                if ($user->privilege == User::PRIVILEGE_ADMIN)
                    $session->set('isAdmin', true);
                
                // Go to home page
                $response->redirect('/');
                return;
            }
        }

        // Return same web if login failed
        $this->setLayout('auth');
        return $this->render('login', [
            'model' => $loginForm
        ]);
    }

    public function register(Request $request, Response $response, Session $session)
    {
        $user = new User();

        if ($request->isPost()) {
            // Load data
            $user->loadData($request->getBody());

            // Check condition
            if ($user->validate() && $user->save()) {
                $session->setFlash('success', 'Hehe cám ơn bạn đã đăng kí nha!');
                $response->redirect('/');
                return;
            }

            // Returns
            return $this->render('register', [
                'model' => $user
            ]);
        }

        $this->setLayout('auth');
        return $this->render('register', [
            'model' => $user
        ]);
    }

    public function profile(Request $request, Response $response, Session $session)
    {
        return $this->render('profile');
    }

    public function logout(Request $request, Response $response, Session $session)
    {
        // Set flash session
        if ($session->isGuest()) $session->setFlash('danger',  'Bạn đã đăng xuất rồi mà bạn còn đăng xuất làm gì nữa, tấu hề à.');
        else                     $session->setFlash('success', 'Bạn đã đăng xuất.');

        // Remove variables
        $session->remove('user');
        $session->remove('isAdmin');
        $session->remove('cart_ids');

        // Goto home
        $response->redirect('/');
    }

    public function editProfile(Request $request, Response $response, Session $session)
    {   
        $profileForm = new ProfileForm();

        if ($request->isPost()) {
            // Load data
            $profileForm->loadData($request->getBody());

            // Check condition
            if($profileForm->validate() && $profileForm->change()) {
                $session->setFlash('success', 'Thay đổi thành công!');
                $response->redirect('/profile');
                return;
            }
            
            // Return
            return $this->render('editProfile', [
                'model' => $profileForm
            ]);
        }

        return $this->render('editProfile', [
            'model' => $profileForm
        ]);
    }

    public function viewCart(Request $request, Response $response, Session $session)
    {
        return $this->render('viewCart');
    }

    public function checkout(Request $request, Response $response, Session $session)
    {
        if ($session->get('cart_ids')) {
            $session->remove('cart_ids');
            $session->setFlash('success', 'Cảm ơn bạn vì đã mua hàng của chúng tôi nha. Nhìn bạn là mình biết bạn rất là đáng yêu rồi ý!');
            $response->redirect('/');
            return;
        }
        
        $session->setFlash('danger', 'Mình xin chân thành không thèm cảm ơn bạn vì đã không mua hàng của chúng mình nha.');
        $response->redirect('/viewCart');
    }

};