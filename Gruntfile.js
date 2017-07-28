module.exports = function(grunt) {
    grunt.initConfig({
        pkg: grunt.file.readJSON('package.json'),
        regex_extract: {
            style: {
                options: {
                    regex: '<!-- extract:style\.html -->((:?.|\n)*?)<!-- endextract -->',
                    includePath : false
                },
                files: {
                    './generated/style.html': ['client.html']
                }
            },
            body: {
                options: {
                    regex: '<!-- extract:body\.html -->((:?.|\n)*?)<!-- endextract -->',
                    includePath : false
                },
                files: {
                    './generated/body.html': ['client.html']
                }
            }
        },
        replace: {
            before: {
                src: 'client.html',
                dest: './generated/client.min.html',
                replacements: [{
                    from: /<!--\s*extract:(.|\n)*?endextract\s?-->/g,
                    to: ''
                }]
            },
            after: {
                src: './generated/*.html',
                dest: './generated/',
                replacements: [{
                    from: /console\.[a-z]+\([^\)]*\);?/g,
                    to: ''
                }, {
                    from: /;[\s\r\n]*\}/g,
                    to: '}'
                }, {
                    from: /"/g,
                    to: '\\"'
                }]
            }
        },
        htmlcompressor: {
            default: {
                files: [{
                    './generated/client.min.html': './generated/client.min.html'
                }, {
                    './generated/style.html': './generated/style.html'
                }, {
                    './generated/body.html': './generated/body.html'
                }],
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
        file_append: {
            default: {
                files: [{
                    prepend: 'HTTP/1.1 200 OK\\r\\nServer: esp8266\\r\\nContent-Type: text/html\\r\\n\\r\\n',
                    input: './generated/client.min.html',
                    output: './generated/client.min.html'
                }]
            }
        }
    });

    grunt.loadNpmTasks('grunt-htmlcompressor');
    grunt.loadNpmTasks('grunt-text-replace');
    grunt.loadNpmTasks('grunt-file-append');
    grunt.loadNpmTasks('grunt-regex-extract');

    grunt.registerTask('default', ['regex_extract', 'replace:before', 'htmlcompressor', 'replace:after', 'file_append']);

};