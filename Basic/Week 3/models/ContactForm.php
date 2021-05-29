<?php

class ContactForm extends Model
{
    
    public string $subject = '';
    public string $email   = '';
    public string $body    = '';

    public function rules(): array
    {
        return [
            'subject' => [self::RULE_REQUIRED],
            'email'   => [self::RULE_REQUIRED],
            'body'    => [self::RULE_REQUIRED],
        ];
    }

    public function getLabels(): array
    {
        return [
            'subject' => 'Chủ đề',
            'email'   => 'Email',
            'body'    => 'Nội dung',
        ];
    }

    public function send()
    {
        return true;
    }

};