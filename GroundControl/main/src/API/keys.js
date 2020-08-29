const prepend = (prefix, obj) => {
    let newObj = {};
    Object.keys(obj).map(key => {
        newObj[key] = `${prefix}/${obj[key]}`;
    })
    return newObj;
}

const APP = prepend('app', {
});

const keys = {
    APP
}

module.exports = {
    default: keys
}