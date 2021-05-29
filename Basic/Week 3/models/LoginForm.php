<?php

class LoginForm extends Model
{

    public string $email    = '';
    public string $password = '';

    public function rules(): array
    {
        return [
            'email'    => [self::RULE_EMAIL, self::RULE_REQUIRED],
            'password' => [self::RULE_REQUIRED, [self::RULE_MIN, 'min' => 8], [self::RULE_MAX, 'max' => 32]]
        ];
    }

    public function getLabels(): array
    {
        return [
            'email'    => 'Email',
            'password' => 'Mật khẩu'
        ];
    }

    public function login()
    {
        // Check if user is found.
        $user = User::findOne(['email' => $this->email]);
        if (!$user) {
            $this->addError('email', 'Email này không tồn tại!');
            return false;
        }

        // Check if password match
        if (!password_verify($this->password, $user->password)) {
            $this->addError('password', 'Mật khẩu xai lè rồi bạn ê!');
            return false;
        }

        // Return the user that the login form received
        return $user;
    }

};