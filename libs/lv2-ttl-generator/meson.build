###############################################################################

lv2_ttl_generator_dependencies = [
]

if not os_windows
    lv2_ttl_generator_dependencies = [
        cc.find_library('dl'),
    ]
endif

lv2_ttl_generator = executable('lv2_ttl_generator',
    sources: [
        'lv2_ttl_generator.cpp'
    ],
    dependencies: lv2_ttl_generator_dependencies,
    install: false,
)

###############################################################################
