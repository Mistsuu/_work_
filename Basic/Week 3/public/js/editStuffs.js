function getUpdateStuffResult(result, args)
{
    result = JSON.parse(result);
    
    // Modify HTML to restore...
    stuff     = result.stuff;
    container = args['container'];
    container.getElementsByClassName('id')[0]          .innerHTML = stuff.id;
    container.getElementsByClassName('name')[0]        .innerHTML = stuff.name;
    container.getElementsByClassName('price')[0]       .innerHTML = stuff.price;
    container.getElementsByClassName('description')[0] .innerHTML = stuff.description;
    container.getElementsByClassName('currency')[0]    .innerHTML = stuff.currency;

    // Display fail message if you want to...
    if (result.status == 'failure') {
        console.log(result.errors);
    }
}

function getRemoveStuffResult(result, args)
{
    result = JSON.parse(result);
    if (result.status == 'success') {
        args['container'].remove();
    } else {
        console.log(result.errors);
    }
}

function getAddStuffResult(result, args)
{
    // Get data...
    result    = JSON.parse(result);
    stuff     = result.stuff;
    container = args['container'];

    // Display fail message if you want to...
    if (result.status == 'failure') {
        console.log(result.errors);
        return;
    }

    // Modify HTML to restore...
    container.getElementsByClassName('id')[0]         .innerHTML = stuff.id;
    container.getElementsByClassName('name')[0]       .innerHTML = stuff.name;
    container.getElementsByClassName('price')[0]      .innerHTML = stuff.price;
    container.getElementsByClassName('description')[0].innerHTML = stuff.description;
    container.getElementsByClassName('currency')[0]   .innerHTML = stuff.currency; 

    // Return the ol' two buttons
    container.getElementsByClassName('edit-button')     [0].removeAttribute('hidden');
    container.getElementsByClassName('remove-button')   [0].removeAttribute('hidden');
    container.getElementsByClassName('removeadd-button')[0].setAttribute('hidden', true);
    container.getElementsByClassName('add-button')      [0].setAttribute('hidden', true);
}

function updateStuff(newStuff, container)
{
    httpPostAsync('/modifyStuff', {'newStuff' : JSON.stringify(newStuff)}, getUpdateStuffResult, {'container' : container});
}

function addStuff(newStuff, container)
{
    httpPostAsync('/addStuff', {'newStuff' : JSON.stringify(newStuff)}, getAddStuffResult, {'container' : container});
}

function removeStuff(container)
{
    var stuffID = container.getElementsByClassName('id')[0].innerHTML;
    httpPostAsync('/removeStuff', {'stuffID' : stuffID}, getRemoveStuffResult, {'container' : container});
}

function toggleOffEditStuffForm(container)
{
    container.getElementsByClassName('edit-button')[0].removeAttribute('hidden');
    container.getElementsByClassName('save-button')[0].setAttribute('hidden', true);

    // Store input field values
    var stuff = {};
    stuff.name        = container.getElementsByClassName('input-name')       [0]['value'];
    stuff.price       = container.getElementsByClassName('input-price')      [0]['value'];
    stuff.description = container.getElementsByClassName('input-description')[0]['value'];    
    stuff.currency    = container.getElementsByClassName('input-currency')   [0]['value'];
    stuff.id          = container.getElementsByClassName('id')               [0].innerHTML;

    // Change stuff
    updateStuff(stuff, container);
}

function toggleOffAddStuffForm(container)
{
    var newStuff = {};
    newStuff.name         = container.getElementsByClassName('input-name')       [0]['value'];
    newStuff.price        = container.getElementsByClassName('input-price')      [0]['value'];
    newStuff.description  = container.getElementsByClassName('input-description')[0]['value'];    
    newStuff.currency     = container.getElementsByClassName('input-currency')   [0]['value'];

    // Add stuff
    addStuff(newStuff, container);
}

function toggleOnAddStuffForm()
{
    // Get template
    var stuffContainerTemplate = document
        .getElementById('stuffListHolder')
        .getElementsByClassName('stuffHolder')
        [0];

    // Clone container
    var cloneContainer = stuffContainerTemplate.cloneNode(true);
    cloneContainer.removeAttribute('hidden');

    // Change every field into an input field
    cloneContainer.getElementsByClassName('id')[0]         .innerHTML = '#';
    cloneContainer.getElementsByClassName('name')[0]       .innerHTML = `<input type="text"     class="form-control     input-name">`;
    cloneContainer.getElementsByClassName('price')[0]      .innerHTML = `<input type="text"     class="form-control     input-price" >`;
    cloneContainer.getElementsByClassName('description')[0].innerHTML = `<input type="text"     class="form-control     input-description"    >`;
    cloneContainer.getElementsByClassName('currency')[0]   .innerHTML = `<input type="text"     class="form-control     input-currency" >`;

    // Replace the two buttons
    cloneContainer.getElementsByClassName('add-button')      [0].removeAttribute('hidden');
    cloneContainer.getElementsByClassName('removeadd-button')[0].removeAttribute('hidden');
    cloneContainer.getElementsByClassName('edit-button')     [0].setAttribute('hidden', true);
    cloneContainer.getElementsByClassName('remove-button')   [0].setAttribute('hidden', true);

    // Add to the end of the stuff list
    document.getElementById('stuffListHolder').appendChild(cloneContainer);
}

function toggleOnEditStuffForm(container)
{
    // Replace the two buttons
    container.getElementsByClassName('save-button')[0].removeAttribute('hidden');
    container.getElementsByClassName('edit-button')[0].setAttribute('hidden', true);

    // Change every field into an input field
    container.getElementsByClassName('name')[0].innerHTML = `<input type="text"     class="form-control     input-name" value="` + container.getElementsByClassName('name')[0].innerHTML + `">`;
    container.getElementsByClassName('price')[0] .innerHTML = `<input type="text"     class="form-control     input-price"  value="` + container.getElementsByClassName('price')[0] .innerHTML + `">`;
    container.getElementsByClassName('description')[0]    .innerHTML = `<input type="text"     class="form-control     input-description"     value="` + container.getElementsByClassName('description')[0]    .innerHTML + `">`;
    container.getElementsByClassName('currency')[0] .innerHTML = `<input type="text"     class="form-control     input-currency"  value="` + container.getElementsByClassName('currency')[0]    .innerHTML + `">`;
}

function updateStuffList(stuffsData)
{
    // Loop through stuffs
    var stuffs = [];
    stuffs = JSON.parse(stuffsData);
    stuffs.forEach((stuff, index) => {
        // Get template
        var stuffContainerTemplate = document
            .getElementById('stuffListHolder')
            .getElementsByClassName('stuffHolder')
            [0];
        
        // Clone container
        var cloneContainer = stuffContainerTemplate.cloneNode(true);
        cloneContainer.removeAttribute('hidden');

        // Set values
        cloneContainer.getElementsByClassName('id')[0]         .innerHTML = stuff.id;
        cloneContainer.getElementsByClassName('name')[0]       .innerHTML = stuff.name;
        cloneContainer.getElementsByClassName('price')[0]      .innerHTML = stuff.price;
        cloneContainer.getElementsByClassName('description')[0].innerHTML = stuff.description;
        cloneContainer.getElementsByClassName('currency')[0]   .innerHTML = stuff.currency;

        // Add to the end of the stuff list
        document.getElementById('stuffListHolder').appendChild(cloneContainer);
    });
}

function removeContainer(container)
{
    container.remove();
}

function getAllStuffs()
{
    httpGetAsync('/getAllStuffs', {}, updateStuffList);
}