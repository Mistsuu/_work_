<?php

class SiteController extends Controller
{

    public function home()
    {
        return $this->render('home');
    }

    public function contact(Request $request, Response $response, Session $session)
    {
        $contactFormModel = new ContactForm();

        if ($request->isPost()) {
            // Load data
            $contactFormModel->loadData($request->getBody());

            // Check condition
            if ($contactFormModel->validate() && $contactFormModel->send()) {
                $session->setFlash('success', 'Ok');
                $response->redirect('/contact');
            }
        }

        // Returns
        return $this->render('contact', [
            'model' => $contactFormModel
        ]);
    }

    public function viewItem(Request $request, Response $response, Session $session)
    {
        // Find stuff
        $stuff = Stuff::findOne([Stuff::getPrimaryKey() => $request->getBody()['itemID']]);
        
        // If found stuff, return display stuff to the user...
        if ($stuff)
            return $this->render('viewItem', [
                'itemName' => $stuff->getName(),
                'itemID'   => $request->getBody()['itemID']
            ]);

        // If not, throw a 404 NOT FOUND ERROR
        throw new NotFoundException();
    }

};