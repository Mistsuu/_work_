<?php

class EditUserForm extends Model
{

    public string $email     = '';
    public string $password  = '';
    public string $firstName = '';
    public string $lastName  = '';
    public string $id        = '';
    public int    $privilege = User::PRIVILEGE_USER;

    public function rules(): array
    {
        return [
            'firstName'       => [self::RULE_REQUIRED],
            'lastName'        => [self::RULE_REQUIRED],
            'email'           => [self::RULE_REQUIRED, self::RULE_EMAIL, [self::RULE_UNIQUE, 'class' => User::class]],
            'password'        => [[self::RULE_MIN, 'min' => 8], [self::RULE_MAX, 'max' => 32]],
            'privilege'       => [self::RULE_REQUIRED],
            'id'              => [self::RULE_REQUIRED],
        ];
    }

    public function getLabels(): array
    {
        return [];
    }

    public function update() 
    {
        // Trying to pull the original user
        $user = User::findOne([User::getPrimaryKey() => $this->{User::getPrimaryKey()}]);
        if (!$user) {
            $this->addError('unknown', 'Tại sao bạn có thể edit được 1 người không tồn tại thế bạn? Bạn hack à :)');
            return false;
        }

        // Modify & update info
        $user->firstName               = $this->firstName;
        $user->lastName                = $this->lastName;
        $user->email                   = $this->email;
        $user->password                = $this->password == '' ? $user->password : $this->password;
        $user->{User::getPrimaryKey()} = $this->id;
        $user->privilege               = $this->privilege;
        $user->save($this->password != '');
        return true;
    }

};