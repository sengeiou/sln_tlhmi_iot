module.exports = function (plop) {
    // controller generator
    plop.setGenerator('HAL device', {
        description: 'Generate HAL device boilerplate code',
        prompts: [
            {
                type: 'list',
                name: 'deviceType',
                message: 'Device type',
                choices: ['input', 'output', 'camera', 'display'],
            },
            {
                type: 'input',
                name: 'deviceName',
                message: 'Device name',
            },
            {
                type: 'list',
                name: 'core',
                message: 'Core',
                choices: ['cm4', 'cm7'],
            },
        ],
        actions: [
            {
                type: 'add',
                path: 'framework/hal/{{deviceType}}/hal_{{deviceType}}_{{snakeCase deviceName}}.c',
                templateFile: 'plop/templates/hal/{{deviceType}}_dev.hbs',
                abortOnFail: false,
            },
            {
                type: 'modify',
                pattern: '/* Add new HAL device declarations here */',
                path: '{{core}}/source/sln_smart_tlhmi_{{core}}.cpp',
                templateFile: 'plop/templates/hal/hal_device_declaration.hbs',
            },
            {
                type: 'modify',
                pattern: '/* Add new HAL device registrations here */',
                path: '{{core}}/source/sln_smart_tlhmi_{{core}}.cpp',
                templateFile: 'plop/templates/hal/hal_device_registration.hbs',
            },
            {
                type: 'modify',
                pattern: '/* Enable new HAL devices here */',
                path: '{{core}}/board/board_define.h',
                templateFile: 'plop/templates/hal/hal_device_enable.hbs',
            },
            // {
            //     type: 'modify',
            //     pattern: '/* Add new HAL device resource initializations here */',
            //     path: 'source/hardware_init.c',
            //     templateFile: 'tools/plop/templates/hal/hardware_init.hbs',
            // },
        ],
    });
};