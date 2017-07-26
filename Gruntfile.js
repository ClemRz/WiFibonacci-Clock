module.exports = function(grunt) {
    grunt.initConfig({
        pkg: grunt.file.readJSON('package.json'),
        clean: ['client.min.html'],
        htmlcompressor: {
            compile: {
                files: {
                    'client.min.html': 'client.html'
                },
                options: {
                    'compress-js': true,
                    'compress-css': true,
                    'remove-intertag-spaces': true,
                    'remove-js-protocol': true,
                    'remove-script-attr': true,
                    'remove-style-attr': true,
                    'remove-quotes': true
                }
            }
        },
        replace: {
            template: {
                src: 'client.min.html',
                dest: 'client.min.html',
                replacements: [{
                    from: /console\.[a-z]+\([^\)]*\);?/g,
                    to: ''
                }, {
                    from: /;[\s\r\n]*\}/g,
                    to: '}'
                },{
                    from: /"/g,
                    to: '\\"'
                }]
            }
        },
        file_append: {
            default_options: {
                files: [{
                    prepend: 'HTTP/1.1 200 OK\\r\\nServer: esp8266\\r\\nContent-Type: text/html\\r\\n\\r\\n',
                    input: 'client.min.html',
                    output: 'client.min.html'
                }]
            }
        }
    });

    grunt.loadNpmTasks('grunt-htmlcompressor');
    grunt.loadNpmTasks('grunt-contrib-clean');
    grunt.loadNpmTasks('grunt-text-replace');
    grunt.loadNpmTasks('grunt-file-append');

    grunt.registerTask('default', ['clean', 'htmlcompressor', 'replace', 'file_append']);

};