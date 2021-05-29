function addToCart(itemID)
{
    httpGetAsync('/addToCart', {'itemID' : itemID}, displayFlashes);
}