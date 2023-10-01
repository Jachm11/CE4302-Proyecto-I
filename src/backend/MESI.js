

const caches = [{
    id: 1, 
    cache_lines: [],
    id: 2, 
    cache_lines: [],
    id: 3, 
    cache_lines: []
}];


const memory = [
    {
        address: 0x0000,
        data: undefined,
    },
    {
        address: 0x0001,
        data: undefined,
    },
    {
        address: 0x0002,
        data: undefined,
    },
    {
        address: 0x0003,
        data: undefined,
    },
    {
        address: 0x0004,
        data: undefined,
    },
    {
        address: 0x0005,
        data: undefined,
    },
];


function isInLocalCache(local_cache, address){
    return local_cache.find((cache_line) => cache_line.address === address);
};

function isInRemoteCache(remote_caches, address){
    
    for(const cache of remote_caches){
        if(cache.find((cache_line) => cache_line.address === address)){
            return true;
        }
    }
    return false;
}


function getCacheRemoteLines(remote_caches, address){
    const remote_caches = [];
    for(const cache of remote_caches){
        const cache_line = cache.find((cache_line) => cache_line.address === address);
        if(cache_line){
            remote_caches.push(cache_line);
        }
    }
    return remote_caches;
}


const STATE = Object.freeze({
    INVALID: "invalid",
    MODIFIED: "modified",
    EXLUSIVE: "exlusive",
    SHARE: "share"
});

function MESI_protocol(operation, address, data, cache_id, caches, memory){


    const local_cache = caches.find((cache) => cache.id == id);
    const remote_caches = caches.filter((cache) => cache.id !== id);


    if(isInLocalCache(local_cache, address)){
        const local_cache_line = local_cache.find((cache_line) => cache_line.address === address);
        if(isInRemoteCache(remote_caches, address)){
            const remote_cache_lines = getCacheRemoteLines(remote_caches, address);
            /**
             * Value is in both local and remote cache
             */
            
            if(operation === 'write'){
                /* 
                 * SHARED -> MODIFIED
                 * SHARED -> INVALID
                 *
                 * local cache: Shared -> Modified
                 * value in remote caches must be invalidated 
                 */
                local_cache_line.state = STATE.MODIFIED;
                for(const remote_cache_line of remote_cache_lines){
                    remote_cache_line.state = STATE.INVALID;
                }
                return;
            } 
            throw new Error('Invalid transaction')


        } else {
            /**
             * the value is not in local cache but not in remote cache 
             *  */ 

            if(operation === 'write'){
                /**
                 * EXLUSIVE -> MODIFIED
                 * 
                 * Write the value in local cache and memory. 
                 * 
                 */

                local_cache_line.state = STATE.MODIFIED;
            }

            if(operation === 'read'){
                /**
                 * 
                 */
            }

            throw new Error('Invalid transaction')
        }
                    
    } else {


        if(isInRemoteCache(remote_caches, address)){
            /**
             * Value is not in local cache but in remote cache
             */
            const remote_cache_lines = getCacheRemoteLines(remote_caches, address);
            if(operation === 'write'){

                local_cache.push({
                    address, 
                    value: data, 
                    state: INVALID
                })




                


            }



        }




    }




        








}




