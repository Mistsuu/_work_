<?php 
    $this->addCssFiles(['viewItem']);
    $this->addJSFiles(['viewItem', 'addToCart']);
    $this->addOnloadBodyFuncs([
        'getStuffByID' => [$itemID]
    ]);
?>

<h2 class='bd-title' id='nameHolder'></h2>

<br><br>
<div class="row">
    <div class="col-md-6 how-img">
        <img src="https://image.ibb.co/dDW27U/Work_Section2_freelance_img1.png" class="rounded-circle img-fluid" alt=""/>
    </div>
    <div class="col-md-6">
        <h3 id="priceHolder"></h3>
        <br>
        <p class="text-muted" id="descriptionHolder"></p>

        <!-- Add to cart button -->
        <div id='addToCartButtonHolder'></div>
    </div>
</div>