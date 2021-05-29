<?php

class ProfileForm extends Model
{

    public string $firstName   = '';
    public string $lastName    = '';
    public string $email       = '';
    public string $newPassword = '';

    public function rules(): array
    {
        return [
            'email'       => [self::RULE_EMAIL, [self::RULE_UNIQUE, 'class' => User::class]],
            'newPassword' => [[self::RULE_MIN, 'min' => 8], [self::RULE_MAX, 'max' => 32]]
        ];
    }

    public function getLabels(): array
    {
        return [
            'firstName'   => 'Họ',
            'lastName'    => 'Tên',
            'email'       => 'Email',
            'newPassword' => 'Mật khẩu mới'
        ];
    }

    public function change()
    {
        // Get user with the same id & update info
        $user = User::findOne([User::getPrimaryKey() => Application::$app->session->get('user')]);
        if ($user) {
            $user->firstName = $this->firstName   == '' ? $user->firstName : $this->firstName;
            $user->lastName  = $this->lastName    == '' ? $user->lastName  : $this->lastName;
            $user->email     = $this->email       == '' ? $user->email     : $this->email;
            $user->password  = $this->newPassword == '' ? $user->password  : $this->newPassword;
            $user->save($this->newPassword != '');
            return true;
        }

        // How...
        $this->addError('unknown', 'Tại sao bạn có thể edit được 1 người không tồn tại thế bạn? Bạn hack à :)');
        return false;
    }

};