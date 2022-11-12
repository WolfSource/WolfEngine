///*
//    Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
//    https://github.com/WolfEngine/WolfEngine
//*/
//
//#pragma once
//
//#ifdef WOLF_STREAM_RIST
//
//#include <wolf.hpp>
//
//struct rist_data_block;
//
//namespace wolf::stream
//{
//    class w_rist_data_block
//    {
//        friend class w_rist;
//    public:
//        W_API w_rist_data_block() noexcept;
//        W_API virtual ~w_rist_data_block();
//        
//        // disable copy constructor
//        w_rist_data_block(const w_rist_data_block&) = delete;
//        // disable copy operator
//        w_rist_data_block& operator=(const w_rist_data_block&) = delete;
//
//        // Move constructor.
//        W_API w_rist_data_block(w_rist_data_block&& p_other) noexcept;      
//        // Move assignment operator.
//        W_API w_rist_data_block& operator=(w_rist_data_block&& p_other) noexcept;
//
//        /**
//         * initialize rist data block
//         * @returns an excpected of void or unexcpected with error message
//         */
//        W_API tl::expected<void, w_result> init();
//
//        /**
//         * set data to rist data block
//         * @param p_data, data of rist
//         * @param p_len, length of data
//         * @returns an excpected of void or unexcpected with error message
//         */
//        W_API tl::expected<void, w_result> set(const void* p_data, size_t p_len);
//
//        /**
//         * set data from rist data block
//         * @param pProcessName the name of process
//         * @returns an excpected of void pointer or unexcpected with error message
//         */
//        W_API tl::expected<const void*, w_result> get() const;
//
//        /**
//         * get len of data
//         * @returns an excpected of length of data or unexcpected with error message
//         */
//        W_API tl::expected<size_t, w_result> get_len() const;
//
//    private:
//        void release() noexcept;
//        tl::expected<rist_data_block*, w_result> get_internal_block();
//   
//        struct priv; 
//        std::unique_ptr<priv> _priv;
//    };
//}
//
//#endif