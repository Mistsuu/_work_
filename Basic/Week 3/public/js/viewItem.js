function addAddToCartButton(userStatus, args)
{
    if (userStatus != 'guest') {
        document.getElementById('addToCartButtonHolder').innerHTML = `
            <br>
            <button type="submit" name="itemID" onclick="addToCart(` + args['itemID'] + `)" class="btn btn-sm btn-outline-secondary" value="` + args['itemID'] + `">Thêm vào giỏ hàng</button>
        `;
    }
}

function updateViewItemPage(response)
{
    // Convert JSON format to JS structure
    stuff = JSON.parse(response);

    // Change title
    document.title = 'SuShop - ' + stuff.name;

    // Display name
    document.getElementById('nameHolder').innerHTML = stuff.name;

    // Display price
    document.getElementById('priceHolder').innerHTML = 'Giá: ' + stuff.price + ' ' + stuff.currency;

    // Display description
    document.getElementById('descriptionHolder').innerHTML = stuff.description;

    // Display 'Add To Cart' button if we're user...
    httpGetAsync('/getUserStatus', {}, addAddToCartButton, {'itemID' : stuff.id});
}

function getStuffByID(itemID)
{
    httpGetAsync('/getStuffByID', {'itemID' : itemID}, updateViewItemPage);
}