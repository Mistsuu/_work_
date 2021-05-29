<?php

class EditStuffForm extends Model
{

    public string $name;
    public string $price;
    public string $description;
    public string $currency;
    public string $id;

    public function rules(): array
    {
        return [
            'name'        => [self::RULE_REQUIRED, [self::RULE_MAX, 'max' => 256], [self::RULE_UNIQUE, 'class' => Stuff::class]],
            'price'       => [self::RULE_REQUIRED],
            'description' => [[self::RULE_MAX, 'max' => 2000]],
            'currency'    => [[self::RULE_MAX, 'max' => 256]]
        ];
    }

    public function getLabels(): array
    {
        return [];
    }

    public function update() 
    {
        // Trying to pull the original stuff
        $stuff = Stuff::findOne([Stuff::getPrimaryKey() => $this->{Stuff::getPrimaryKey()}]);
        if (!$stuff) {
            $this->addError('unknown', 'Tại sao bạn có thể edit được 1 vật không tồn tại thế bạn? Hách kẻ à :)');
            return false;
        }

        // Check if the stuff's price is numeric
        if (!is_numeric($this->price)) {
            $this->addError('price', 'Giá là số không phải chữ nhoa, đây là giờ toán không phải giờ ngữ vănn');
            return false;
        }

        // Modify & update info
        $stuff->name                     = $this->name;
        $stuff->price                    = $this->price;
        $stuff->description              = $this->description;
        $stuff->currency                 = $this->currency;
        $stuff->{Stuff::getPrimaryKey()} = $this->id;
        
        $stuff->save();
        return true;
    }

};