var totalPrices = {};

function updateAgain()
{
    displayFlashes();
    getCartIDs();
}

function removeFromCart(itemIndex)
{
    httpGetAsync('/removeFromCart', {'itemIndex' : itemIndex}, updateAgain);
}

function updateView(stuff, args)
{
    // Decode JSON
    stuff = JSON.parse(stuff);
    
    // Change HTML
    var container = document.getElementById('cartHolder');
    container.innerHTML += `
        <tr>
            <td><img src="https://dummyimage.com/50x50/55595c/fff" /> </td>
            <td>` + stuff.name + `</td>
            <td></td>
            <td></td>
            <td class="text-right">
                <button class="btn btn-sm btn-danger" onClick="removeFromCart(` + args['itemIndex'] + `)"><i class="fa fa-trash"></i></button> 
            </td>
            <td class="text-right">` + stuff.price + ' ' + stuff.currency + `</td>
        </tr>
    \n`;

    // Add to total price
    if (!totalPrices[stuff.currency]) totalPrices[stuff.currency]  = parseInt(stuff.price);
    else                              totalPrices[stuff.currency] += parseInt(stuff.price);
}

function handleItemIDs(itemIDs)
{
    itemIDs = JSON.parse(itemIDs);
    itemIDs.forEach(getStuffByID);

    var totalAmount = "";
    for (var currency in totalPrices) {
        totalAmount += totalPrices[currency] + ' ' + currency + ' + ';
    }
    totalAmount = totalAmount.substr(0, totalAmount.length - 3);

    var container = document.getElementById('cartHolder');
    container.innerHTML += `
        <tr>
            <td> </td>
            <td><strong>Tổng cộng</strong></td>
            <td> </td>
            <td> </td>
            <td> </td>
            <td class="text-right">` + totalAmount + `</td>
        </tr>
    \n`;
}

function getStuffByID(itemID, index)
{
    httpGetBlocking('/getStuffByID', {'itemID' : itemID}, updateView, {itemIndex: index});
}

function getCartIDs()
{
    totalPrices = {};
    document.getElementById('cartHolder').innerHTML = '';
    httpGetBlocking('/getCartIDs', {}, handleItemIDs);
}