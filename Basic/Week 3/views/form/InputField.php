<?php

class InputField extends BaseField
{
    
    public Model  $model;
    public string $attribute = '';
    public string $type      = 'text';

    public function __construct(Model $model, string $attribute)
    {
        parent::__construct($model, $attribute);
    }

    public function setType($type)
    {
        $this->type = $type;
        return $this;
    }

    public function renderInput(): string
    {
        return sprintf('<input type="%s" name="%s" value="%s" class="form-control%s">', 
            $this->type,
            $this->attribute,
            $this->model->{$this->attribute},
            $this->model->hasError($this->attribute) ? ' is-invalid' : '',
        );
    }

};