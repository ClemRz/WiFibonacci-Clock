module.exports = function(grunt) {
    grunt.initConfig({
        pkg: grunt.file.readJSON('package.json'),
        regex_extract: {
            script: {
                options: {
                    regex: '/\\* extract:script\.js \\*/((:?.|\n)*?)/\\* endextract \\*/',
                    includePath : false
                },
                files: {
                    './generated/script.js': ['client.html']
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
            },
            style: {
                options: {
                    regex: '<!-- extract:style\.html -->((:?.|\n)*?)<!-- endextract -->',
                    includePath : false
                },
                files: {
                    './generated/style.html': ['client.html']
                }
            }
        },
        replace: {
            before: {
                src: 'client.html',
                dest: './generated/payload.html',
                replacements: [{
                    from: /<!--\s*extract:(.|\n)*?endextract\s?-->/g,
                    to: ''
                }, {
                    from: /\/\* extract:script\.js \*\/(.|\n)*?\/\* endextract \*\//g,
                    to: ''
                }]
            },
            after: {
                src: './generated/*.*',
                dest: './generated/',
                replacements: [{
                    from: /console\.[a-z]+\([^\)]*\)(;|,)?/g,
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
                    './generated/payload.html': './generated/payload.html'
                }, {
                    './generated/body.html': './generated/body.html'
                }, {
                    './generated/style.html': './generated/style.html'
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
        uglify: {
            default: {
                files: {
                    './generated/script.js': './generated/script.js'
                }
            }
        },
        copy: {
            default: {
                files: [{
                    src: './generated/payload.html',
                    dest: './generated/test/payload.html'
                }]
            }
        }
    });

    grunt.loadNpmTasks('grunt-htmlcompressor');
    grunt.loadNpmTasks('grunt-text-replace');
    grunt.loadNpmTasks('grunt-regex-extract');
    grunt.loadNpmTasks('grunt-contrib-uglify');
    grunt.loadNpmTasks('grunt-contrib-copy');

    grunt.registerTask('default', ['regex_extract', 'replace:before', 'htmlcompressor', 'uglify', 'copy', 'replace:after']);

};