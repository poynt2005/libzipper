{
    "targets": [
        {
            "target_name": "Zipper",
            "cflags!": ["-fno-exceptions"],
            "cflags_cc!": ["-fno-exceptions"],
            "sources": ["lib/native/Binding.cc"],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
                "<!@(node -p \"require('./path').zipperIncl\")"
            ],
            'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
            'libraries': ["<!@(node -p \"require('./path.js').zipperLib\")"],
            'msvs_settings': {
                'VCCLCompilerTool': {
                    'AdditionalOptions': ['-std:c++20', ]
                },
            }
        }
    ]
}
