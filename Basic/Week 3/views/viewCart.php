<?php
    $this->setTitle('Giỏ hàng');
    $this->addCSSFiles(['viewCart']);
    $this->addJSFiles(['viewCart']);
    $this->addOnloadBodyFuncs([
        'getCartIDs' => []
    ]);
?>

<section class="jumbotron text-center">
    <div class="container">
        <h1 class="jumbotron-heading">Giỏ hàng</h1>
     </div>
</section>

<br>

<div class="container mb-4">
    <div class="row">
        <div class="col-12">
            <div class="table-responsive">
                <table class="table table-striped">
                    <thead>
                        <tr>
                            <th scope="col"> </th>
                            <th scope="col">Sản phẩm</th>
                            <th scope="col"></th>
                            <th scope="col" class="text-center"></th>
                            <th> </th>
                            <th scope="col" class="text-right">Giá</th>
                        </tr>
                    </thead>
                    <!-- Cart details here -->
                    <tbody id="cartHolder"></tbody>
                </table>
            </div>
        </div>
        <div class="col mb-2">
            <div class="row">
                <div class="col-sm-12  col-md-6">
                    <?php Form::begin('/', 'get'); ?>
                    <button class="btn btn-block btn-light">Tiếp tục lượn lờ</button>
                    <?php Form::end(); ?>
                </div>
                <div class="col-sm-12 col-md-6 text-right">
                    <?php Form::begin('/checkout', 'get'); ?>
                    <button class="btn btn-block btn-success">Thanh toán</button>
                    <?php Form::end(); ?>
                </div>
            </div>
        </div>
    </div>
</div>